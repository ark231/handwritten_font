#include <fmt/core.h>
#include <hpdf.h>

#include <algorithm>
#include <boost/program_options.hpp>
#include <filesystem>
#include <iostream>
#include <map>
#include <string>
#include <toml.hpp>
#include <vector>

#include "chardef/chardef_dirmeta.hpp"
#include "chardef/chardef_filemeta.hpp"
#include "chardef/parse_chardef.hpp"
#include "general/helpers.hpp"
#include "input_pdf_consts.hpp"
#include "input_pdf_generator.hpp"

namespace bpo = boost::program_options;
namespace stdfsys = std::filesystem;

std::vector<int> parse_ranges(toml::value, std::string, std::string, std::string);
void add_pages(handfont::input_pdf_generator&, std::vector<int>, handfont::FileCode, handfont::ChardefDirmeta);

int main(int argc, char* argv[]) {
    bpo::options_description opt("option");
    opt.add_options()("help,h", "show this help")
        /*
        ("grid_size,g",bpo::value<std::string>(),"枠のサイズ 必須")
        ("font_type,f",bpo::value<std::string>(),"フォントのタイプ 必須")
        ("file_id,i",bpo::value<std::string>(),"ファイルid 必須")
        */
        ("chardefset_rootdir,d", bpo::value<std::vector<std::string>>()->composing(),
         "文字定義ファイルセットの追加検索パス")("project_file,p", bpo::value<std::string>(), "プロジェクトファイル");
    bpo::variables_map varmap;
    bpo::store(bpo::parse_command_line(argc, argv, opt), varmap);
    bpo::notify(varmap);
    if (varmap.count("help")) {
        std::cout << opt << std::endl;
        std::exit(EXIT_SUCCESS);
    }
    // if(!varmap.count("grid_size") || !varmap.count("font_type") || !varmap.count("file_id")){
    if (!varmap.count("project_file")) {
        std::cout << "error: not enough arguments" << std::endl;
        std::cout << opt << std::endl;
        std::exit(EXIT_FAILURE);
    }
    /*
    auto grid_size = varmap["grid_size"].as<std::string>();
    auto font_type = varmap["font_type"].as<std::string>();
    auto file_id = varmap["file_id"].as<std::string>();
    */

    auto project_filepath = varmap["project_file"].as<std::string>();
    const auto project_data = toml::parse(project_filepath);
    auto fontname = toml::find<std::string>(project_data, "fontname");
    auto font_type = toml::find<std::string>(project_data, "fonttype");
    auto grid_size = toml::find<std::string>(project_data, "gridsize");
    std::map<std::string, std::map<std::string, bool>> required;
    required["small"]["mono"] = false;
    required["small"]["proportional"] = false;
    required["large"]["mono"] = false;
    required["large"]["proportional"] = false;
    if (grid_size == "both") {
        if (font_type == "both") {
            required["small"]["mono"] = true;
            required["small"]["proportional"] = true;
            required["large"]["mono"] = true;
            required["large"]["proportional"] = true;
        } else {
            required["small"][font_type] = true;
            required["large"][font_type] = true;
        }
    } else {
        if (font_type == "both") {
            required[grid_size]["mono"] = true;
            required[grid_size]["proportional"] = true;
        } else {
            required[grid_size][font_type] = true;
        }
    }

    std::vector<stdfsys::path> chardefset_rootdirs{};
    if (varmap.count("chardefset_rootdir")) {
        chardefset_rootdirs = varmap["chardefset_rootdir"].as<std::vector<stdfsys::path>>();
    }
    chardefset_rootdirs.push_back(handfont::get_self_dir().parent_path() / "chardefs");
    auto chardefset_data = toml::find_or(project_data, "chardefset", {{"id", "standard"}, {"version", 0}});
    auto id = toml::find<std::string>(chardefset_data, "id");
    auto version = toml::find<int>(chardefset_data, "version");
    auto file_rootdir = handfont::ChardefDirmeta::from_id(chardefset_rootdirs, id, version);
    if (not file_rootdir.is_valid()) {
        fmt::print(stderr, R"(error: chardefset "{}"v{} not found)", id, version);
        std::exit(EXIT_FAILURE);
    }

    handfont::input_pdf_generator result(fontname);
    if (required.count("small")) {
        if (required["small"]["mono"]) {
            handfont::FileCode input_code = "SM";
            add_pages(result, parse_ranges(project_data, file_rootdir.path(), "small", "mono"), input_code,
                      file_rootdir);
        }
        if (required["small"]["proportional"]) {
            handfont::FileCode input_code = "SP";
            add_pages(result, parse_ranges(project_data, file_rootdir.path(), "small", "proportional"), input_code,
                      file_rootdir);
        }
    }
    if (required.count("large")) {
        if (required["large"]["mono"]) {
            handfont::FileCode input_code = "LM";
            add_pages(result, parse_ranges(project_data, file_rootdir.path(), "large", "mono"), input_code,
                      file_rootdir);
        }
        if (required["large"]["proportional"]) {
            handfont::FileCode input_code = "LP";
            add_pages(result, parse_ranges(project_data, file_rootdir.path(), "large", "proportional"), input_code,
                      file_rootdir);
        }
    }

    stdfsys::path outpath = stdfsys::path(project_filepath).parent_path() / (fontname + ".pdf");
    result.saveto_file(outpath.native());
    return 0;
}

std::vector<int> parse_ranges(toml::value project_data, std::string file_rootdir, std::string size, std::string type) {
    std::vector<int> result;
    auto chardef_range = toml::find(toml::find(toml::find(project_data, "chardefs"), size), type);
    if (auto set = toml::find<std::string>(chardef_range, "set"); set == "all") {
        result = handfont::search_available_files(file_rootdir, size, type);
    } else if (set == "custom") {
        auto id_groups = toml::find<std::vector<toml::value>>(chardef_range, "id_groups");
        for (const auto& id_group : id_groups) {
            auto begin = toml::find<int>(id_group, "begin");
            auto end = toml::find<int>(id_group, "end");
            for (int id = begin; id <= end; id++) {
                result.push_back(id);
            }
        }
    }
    std::sort(result.begin(), result.end());
    return result;
}

void add_pages(handfont::input_pdf_generator& result, std::vector<int> file_ids, handfont::FileCode code_head,
               handfont::ChardefDirmeta file_rootdir) {
    for (const auto& file_id : file_ids) {
        auto input_code = code_head + "_" + handfont::to_hex(file_id, 4, '0');
        handfont::chardef_filemeta input_filemeta(input_code, file_rootdir);
        result.add_page(input_filemeta);
    }
}
