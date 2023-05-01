#include "image_handler.hpp"

#include <spdlog/spdlog.h>

#include <algorithm>
#include <cassert>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>
#include <regex>
#include <stdexcept>
#include <string>
#include <vector>

#include "chardef/chardef_convert_consts.hpp"
#include "chardef/parse_chardef.hpp"
#include "general/helpers.hpp"
#include "read_metadata.hpp"
namespace handfont {
bool is_empty(cv::Mat, double);
image_handler::image_handler(stdfsys::path project_dir, stdfsys::path file_rootdir, stdfsys::path image_filepath) {
    this->project_dir = project_dir;
    this->file_rootdir = file_rootdir;
    path_image = image_filepath;
    if (stdfsys::exists(image_filepath.replace_extension(".toml"))) {
        spdlog::info("error file found");
        this->has_data_file = true;
        error_filedata = toml::parse(image_filepath.replace_extension(".toml"));
        auto general_data = toml::find(error_filedata, "general");
        dpmm = toml::find<double>(general_data, "dpmm");
        auto TL_data = toml::find<std::string>(general_data, "TL_data");
        if (TL_data[2] == '_') {  // TL_Handfont~~みたいな場合
            TL_data = TL_data.substr(3);
        }
        auto markers_data = toml::find(error_filedata, "markers");
        std::vector<corner> corner_datas;
        for (const std::string& corner_name : {"TL", "TR", "BR", "BL"}) {
            auto corner_data = toml::find(markers_data, corner_name);
            auto corner_x = toml::find<int>(corner_data, "x");
            auto corner_y = toml::find<int>(corner_data, "y");
            qr_code qr_tmp;
            qr_tmp.data = corner_name + "_" + TL_data;
            corner_datas.push_back(corner(globalPoint(cv::Point(corner_x, corner_y)), qr_tmp));
        }
        write_handler = write_area(corner_datas);
    } else {
        this->has_data_file = false;
        handfont::image_metadata input_meta(path_image);
        dpmm = input_meta.get_dpmm();
    }
    spdlog::debug(dpmm);
    block_size = 1.0 * dpmm;
}
void image_handler::find_bbox(cv::Mat& image) {
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(image, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);
    num_contours = contours.size();
    for (const auto& contour : contours) {
        auto bound_rect_tmp = cv::boundingRect(contour);
        /*ignore too small or too big rectangles*/
        if (15 * dpmm * 15 * dpmm < bound_rect_tmp.area() &&
            bound_rect_tmp.area() < image_blue.rows * image_blue.cols * (90 / 100.0)) {
            boundRects.push_back(bound_rect_tmp);
        }
    }
}
void image_handler::find_and_decode_qr_marker(cv::Mat& image_bin) {
    cv::QRCodeDetector qr_detector;
    std::vector<std::string> qr_infos;
    std::vector<std::vector<cv::Point>> qr_vertices;  // multi 'multi vertex'
    spdlog::info("start detecting qr codes and corner marcers");
    int counter_corner = 0;
    int counter_qr = 0;
    for (const auto& boundRect : boundRects) {
        cv::Point origin(boundRect.x, boundRect.y);
        cv::Mat qr_area(image_bin, boundRect);
        cv::Mat qr_area_border(qr_area.rows + 2 * (5.0 * dpmm), qr_area.cols + 2 * (5.0 * dpmm), CV_8UC1);
        cv::copyMakeBorder(qr_area, qr_area_border, 5.0 * dpmm, 5.0 * dpmm, 5.0 * dpmm, 5.0 * dpmm, cv::BORDER_CONSTANT,
                           0xff);
        handfont::qr_code code_tmp;
        std::vector<cv::Point> code_vertice;
        code_tmp.data = qr_detector.detectAndDecode(qr_area_border, code_vertice);
        if (code_vertice.size() == 4) {
            std::vector<handfont::globalPoint> code_vertice_local;
            for (const auto& vertex : code_vertice) {
                code_vertice_local.push_back((handfont::globalPoint)handfont::localPoint(vertex, origin));
            }
            code_tmp.vertice = code_vertice_local;
            qr_codes.push_back(code_tmp);
            counter_qr++;
            continue;
        }
        /*check if a corner marcer is in the area*/
        cv::Mat corner_area(image_blue, boundRect);
        cv::Mat corner_area_bin(corner_area.size(), CV_8UC1);
        cv::threshold(corner_area, corner_area_bin, 0, 0xff, cv::THRESH_BINARY | cv::THRESH_OTSU);
        cv::Mat corner_edge(image_blue, boundRect);
        cv::Mat corner_edge_color;
        cv::cvtColor(corner_edge, corner_edge_color, cv::COLOR_GRAY2BGR);

        std::vector<std::vector<cv::Point>> corner_contours;
        std::vector<cv::Vec4i> hierarchy;
        cv::findContours(corner_area_bin, corner_contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);
        std::vector<std::vector<cv::Point>> contours_poly;
        contours_poly.reserve(num_contours);
        markers_sides.reserve(num_contours);
        for (const auto& contour : corner_contours) {
            std::vector<cv::Point> contour_poly;
            cv::approxPolyDP(contour, contour_poly, cv::arcLength(contour, true) * (3 / 100.0), true);
            if (10 * dpmm * 10 * dpmm < cv::contourArea(contour_poly) &&
                cv::contourArea(contour_poly) < corner_edge.rows * corner_edge.cols * (90 / 100.0)) {
                if (contour_poly.size() == 6) {
                    contours_poly.push_back(contour_poly);
                    std::vector<handfont::side<handfont::localPoint>> marker_sides;
                    for (int i = 0; i < 6; i++) {
                        handfont::localPoint start(contour_poly[i], origin);
                        handfont::localPoint end(contour_poly[(i + 1) % 6], origin);
                        marker_sides.push_back(handfont::side<handfont::localPoint>(start, end));
                    }
                    markers_sides.push_back(marker_sides);
                }
            }
        }
        spdlog::debug("{} contours found", std::to_string(contours_poly.size()));
    }
}
void image_handler::out_error_file() {
    if (error_filedata.is_uninitialized()) {
        toml::value squeezed_data;
        for (const auto& table_name : {"markers", "general"}) {
            squeezed_data[table_name] = toml::value();
        }
        squeezed_data["general"]["TL_data"] = write_handler.get_TL_data();
        squeezed_data["general"]["dpmm"] = dpmm;
        for (const auto& corner : {"TL", "TR", "BR", "BL"}) {
            squeezed_data["markers"][corner] = toml::value();
            auto corner_datas = write_handler.get_corners();
            squeezed_data["markers"][corner]["x"] = corner_datas[corner].point.x;
            squeezed_data["markers"][corner]["y"] = corner_datas[corner].point.y;
        }
        error_filedata = squeezed_data;
    }
    std::ofstream error_filestream;
    error_filestream.open(path_image.replace_extension(".toml"));
    if (!error_filestream) {
        auto flname = path_image.replace_extension(".toml").native();
        throw std::runtime_error("error: couldn't open " + flname + "\n" + toml::format(error_filedata));
    }
    error_filestream << std::setw(60) << error_filedata;
    error_filestream.close();
}

void image_handler::writeto_file(stdfsys::path outfilepath, cv::Mat image) {
    try {
        cv::imwrite(outfilepath.native(), image);
    } catch (cv::Exception excep) {
        spdlog::error("couldn't write image to " + outfilepath.native() + "\n{}", excep.what());
        std::exit(EXIT_FAILURE);
    }
    spdlog::info("successfully written into " + outfilepath.native());
}

void image_handler::process() {
    auto image = cv::imread(path_image.native());
    if (image.empty()) {
        spdlog::error("couldn't load input image file " + path_image.native());
        return;
    }
    spdlog::info("successfully loaded input image " + path_image.native());
    image_blue = cv::Mat(image.rows, image.cols, CV_8UC1);
    if (image.channels() != 1) {
        cv::extractChannel(image, image_blue, 0);
    } else {
        image_blue = image;
    }
    cv::Mat image_bin(image.rows, image.cols, CV_8UC1);
    spdlog::info("start trying to binalize");
    cv::threshold(image_blue, image_bin, 0, 0xff, cv::THRESH_BINARY | cv::THRESH_OTSU);
    spdlog::info("finish binalization");

    cv::Mat image_dialate(image.size(), CV_8UC1);
    cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(block_size, block_size));
    cv::erode(image_bin, image_dialate, element);

    if (!has_data_file) {
        find_bbox(image_dialate);
        find_and_decode_qr_marker(image_bin);

        spdlog::debug("{} qr codes found", std::to_string(qr_codes.size()));
        spdlog::debug("{} markers found", std::to_string(markers_sides.size()));
        // 最悪TLさえ読み取れれば後は補完できる
        while (qr_codes.size() < 4) {
            handfont::qr_code filler;
            qr_codes.push_back(filler);
        }
        if (markers_sides.size() < 4) {
            spdlog::error("couldn't detect enough corner markers in the input image!");
            out_error_file();
            return;
        }
        std::vector<handfont::corner> corners;
        for (auto& marker : markers_sides) {
            // マーカーの辺を長さ順にソート
            std::sort(marker.begin(), marker.end());
            handfont::corner corner_tmp;
            // 3番目、四番目に長い辺の共有点を取得
            try {
                corner_tmp.point = (handfont::globalPoint)handfont::same_points(marker[3 - 1], marker[4 - 1]);
            } catch (std::runtime_error excep) {
                spdlog::error("couldn't find corner");
                spdlog::debug(excep.what());
                return;
            }
            size_t closest_idx_qr;
            double current_dist =
                std::numeric_limits<double>::infinity();  // 無効である値かつどんな有効な値よりも「大きい」
            // 最も近いqrコードを取得
            for (size_t idx_qr = 0; const auto& qr_code : qr_codes) {
                double new_dist = cv::norm(qr_code.center() - (cv::Point)corner_tmp.point);
                if (current_dist > new_dist) {  // 近い<=>距離が小さい!!!
                    current_dist = new_dist;
                    closest_idx_qr = idx_qr;
                }
                idx_qr++;
            }
            // 確定したqrコードは候補から外す
            corner_tmp.code = qr_codes[closest_idx_qr];
            qr_codes.erase(qr_codes.begin() + closest_idx_qr);
            corners.push_back(corner_tmp);
        }
        this->write_handler = write_area(corners);
        if (!this->write_handler.is_valid()) {
            spdlog::error("couldn't decode TL qr code");
            out_error_file();
            return;  // 切り分けたときは、例外を投げて、process関数の方でキャッチしたときにreturnするように
        }
    }
    spdlog::debug(write_handler.get_TL_data());
    auto image_write_area = write_handler.centerize_image(image_blue);
    spdlog::info("start cutting off characters");
    std::smatch match_filecode;
    auto TL_data = write_handler.get_TL_data();
    std::regex_search(TL_data, match_filecode, std::regex("[SL][MP]_[0-9]+"));
    handfont::chardef_filemeta filemeta(match_filecode.str(), file_rootdir);
    handfont::chardef_data chardef;
    chardef.parse_chardef(filemeta);
    int num_internal_cols;
    int num_internal_rows;
    double write_grid_ratio;
    if (chardef.size == handfont::grid_size::SMALL) {
        num_internal_cols = 24 * 2;  // 48
        if (chardef.is_Fixed_Base) {
            num_internal_rows = ((30 / 2) / 5) * 4;  // 12
            write_grid_ratio = (3 / 5.0);
        } else {
            num_internal_rows = 30 / 2;  // 15
            write_grid_ratio = (1 / 2.0);
        }
    } else if (chardef.size == handfont::grid_size::LARGE) {
        num_internal_cols = 24;
        if (chardef.is_Fixed_Base) {
            num_internal_rows = ((30 - 30 % 4) / 4);  // 7
            write_grid_ratio = (3 / 4.0);
        } else {
            num_internal_rows = 30 / 3;  // 10
            write_grid_ratio = (2 / 3.0);
        }
    }
    spdlog::debug("start making internal grids");
    double width_internal_grid = image_write_area.cols / (double)num_internal_cols;
    double height_internal_grid = image_write_area.rows / (double)num_internal_rows;
    std::vector<cv::Rect> internal_grids;
    internal_grids.reserve(num_internal_cols * num_internal_rows);
    for (size_t row = 0; row < num_internal_rows; row++) {
        for (size_t col = 0; col < num_internal_cols; col++) {
            internal_grids.push_back(cv::Rect(
                // width_internal_grid*col,
                (image_write_area.cols * col) / (double)num_internal_cols,
                // height_internal_grid*row,
                (image_write_area.rows * row) / (double)num_internal_rows, width_internal_grid, height_internal_grid));
        }
    }
    auto cache_dir = filemeta.dir_order(project_dir / ".cache");
    if (!stdfsys::exists(cache_dir)) {
        stdfsys::create_directories(cache_dir);
    }
    auto metafile_path = cache_dir / ".metadata.toml";
    if (stdfsys::exists(metafile_path)) {
        metadata = toml::parse(metafile_path.native());
    }
    for (const std::string& key : {"general", "chars", "alters"}) {
        if (metadata.is_uninitialized() || !metadata.contains(key)) {
            metadata[key] = toml::value{{"default", true}};  // 初期値
            if (key == std::string("general")) {
                metadata["general"]["has_lower_latin"] = false;
            }
        }
    }
    std::ofstream metafile_stream;
    metafile_stream.open(metafile_path);
    if (!metafile_stream) {
        spdlog::error("couldn't open metafile: " + metafile_path.native());
        exit(EXIT_FAILURE);
    }
    metadata["general"]["is_Fixed_Base"] = chardef.is_Fixed_Base;
    spdlog::debug("start processing internal grids");
    double constexpr units_per_em = 1024;
    // double constexpr units_per_em_per_inch = (1.0*handfont::inch_per_mm)/(7.0/units_per_em);
    // double constexpr units_per_em_per_inch = (units_per_em/7.0)*handfont::mm_per_inch;
    double units_per_mm;
    if (chardef.size == handfont::grid_size::SMALL) {
        units_per_mm = (units_per_em / 7.0);
    } else if (chardef.size == handfont::grid_size::LARGE) {
        units_per_mm = (units_per_em / 14.0);
    }
    spdlog::debug("{}units/mm", units_per_mm);
    std::cout << units_per_mm << "units/mm" << std::endl;
    auto internal_grid = internal_grids.begin();
    std::vector<toml::value> alters;
    for (const auto& char_info : chardef.char_infos) {
        if (char_info.g_type == handfont::guide_type::LOWER_LATIN) {
            metadata["general"]["has_lower_latin"] = true;
        }
        cv::Rect unified_grid;
        auto code_point = "U" + handfont::to_hex(char_info.character, 4, '0');
        if (char_info.width == handfont::char_width::HALF) {
            unified_grid = *internal_grid;
            internal_grid++;
            metadata["chars"][code_point]["width"] = "HALF";
        } else if (char_info.width == handfont::char_width::FULL) {
            if (std::distance(internal_grids.begin(), internal_grid) % num_internal_cols == num_internal_cols - 1) {
                internal_grid++;  // 端が余ったら飛ばす
            }
            unified_grid = *internal_grid | *(internal_grid + 1);
            internal_grid += 2;
            metadata["chars"][code_point]["width"] = "FULL";
        }
        metadata["chars"][code_point]["guide_type"] = handfont::to_string(char_info.g_type);
        cv::Mat image_unified_grid(image_write_area, unified_grid);
        cv::Rect info_grid(0, 0, unified_grid.width, unified_grid.height * (1 - write_grid_ratio));
        cv::Rect write_grid(0, unified_grid.height * write_grid_ratio, unified_grid.width,
                            unified_grid.height * write_grid_ratio);
        cv::Mat image_info_grid(image_unified_grid, info_grid);
        cv::Mat image_write_grid(image_unified_grid, write_grid);
        cv::Mat image_write_grid_resized(image_unified_grid, write_grid);
        if (units_per_mm > dpmm) {
            cv::resize(image_write_grid, image_write_grid_resized, cv::Size(), (units_per_mm / dpmm),
                       (units_per_mm / dpmm), cv::INTER_CUBIC);
        } else {
            image_write_grid_resized = image_write_grid;
        }
        cv::Mat image_write_grid_bin(image_write_grid_resized.size(), CV_8UC1);
        cv::threshold(image_write_grid_resized, image_write_grid_bin, 0, 0xff, cv::THRESH_BINARY | cv::THRESH_OTSU);
        // かすれ除去
        spdlog::debug("start unti-noise processes");
        cv::Mat image_write_grid_closed(image_write_grid_bin.size(), CV_8UC1);
        auto element = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5));
        cv::morphologyEx(image_write_grid_bin, image_write_grid_closed, cv::MORPH_CLOSE, element);
        cv::Mat image_write_grid_opened(image_write_area.rows, image_write_area.cols, CV_8UC1);
        element = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(10, 10));
        cv::morphologyEx(image_write_grid_closed, image_write_grid_opened, cv::MORPH_OPEN, element);
        cv::Mat result = image_write_grid_opened;
        if (is_empty(result, units_per_em)) {
            if (char_info.has_alternative) {
                auto alter_data = char_info.alternative;
                toml::value alter;
                auto src_path = alter_data.filemeta.dir_order(project_dir / ".cache") /
                                (to_hex(alter_data.character, 4, '0') + ".png");
                alter["src"] = src_path.native();
                alter["dst"] = (cache_dir / (to_hex(char_info.character, 4, '0') + ".png")).native();
                alters.push_back(alter);
                spdlog::info("{}: alternative found", to_hex(char_info.character, 4, '0'));
            }
            continue;
        }
        /*
         * 代替文字、不使用指定などを検知する処理を後で追加する
         */
        writeto_file((cache_dir / (handfont::to_hex(char_info.character, 4, '0') + ".png")), result);
    }
    metadata["alters"] = alters;
    metafile_stream << std::setw(80) << metadata;
    metafile_stream.close();
}
bool is_empty(cv::Mat image, double dpmm) {
    auto image_rev = ~image;
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(image_rev, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);
    bool is_empty = true;
    for (const auto& contour : contours) {
        if (cv::contourArea(contour) > 0.15 * dpmm * 0.15 * dpmm) {
            is_empty = false;
            break;
        }
    }
    return is_empty;
}
}  // namespace handfont
