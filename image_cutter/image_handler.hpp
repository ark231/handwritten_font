#ifndef HANDFONT_IMAGE_HANDLER
#define HANDFONT_IMAGE_HANDLER
#include <filesystem>
#include <opencv2/core.hpp>
#include <toml.hpp>

#include "image_includes.hpp"
namespace stdfsys = std::filesystem;
namespace handfont {
class image_handler {
    bool has_data_file;
    double dpmm;
    double block_size;
    std::vector<handfont::qr_code> qr_codes;
    std::vector<std::vector<handfont::side<handfont::localPoint>>> markers_sides;
    stdfsys::path project_dir;
    stdfsys::path file_rootdir;
    stdfsys::path path_image;
    stdfsys::path output_path;
    std::vector<cv::Rect> boundRects;
    handfont::write_area write_handler;
    toml::value error_filedata;
    toml::value metadata;
    cv::Mat image_blue;  // 最終的にこれを切り出してどうこうするから、結局最後までこれが必要になる
    int num_contours;
    void find_bbox(cv::Mat&);
    void find_and_decode_qr_marker(cv::Mat&);
    void writeto_file(stdfsys::path outfilepath, cv::Mat image);
    void out_error_file();

   public:
    image_handler(stdfsys::path project_dir, stdfsys::path file_rootdir, stdfsys::path image_filepath);
    void process();
};
}  // namespace handfont
#endif
