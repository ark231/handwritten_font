#include <spdlog/spdlog.h>

#include <boost/program_options.hpp>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "general/helpers.hpp"
#include "image_handler.hpp"
namespace bpo = boost::program_options;
namespace stdfsys = std::filesystem;

void writeto_file(stdfsys::path outfilepath, cv::Mat);

int main(int argc, char* argv[]) {
    bpo::options_description opt("option");
    opt.add_options()("help,h", "show this help")(
        "chardef_dir,d",
        bpo::value<std::string>()->default_value(handfont::get_self_dir().parent_path().native() + "/chardefs"),
        "文字定義ファイルのあるディレクトリ")("project_file,p", bpo::value<std::string>(), "プロジェクトファイル")
        //("dpi,i",bpo::value<handfont::dpi>(),"入力画像のスケール")
        ("debug,b", "デバッグ用出力を有効化する");
    // for experiment
    //("block_size,s",bpo::value<int>(),"size of binarize block")
    //("constant,c",bpo::value<int>(),"constant")
    //("threshold,t",bpo::value<int>(),"threshold for edge detection")
    //("diameter,a",bpo::value<int>(),"size of blur kernel");
    /*
    ("sigma_color,b",bpo::value<int>(),"プロジェクトファイル")
    ("sigma_space,e",bpo::value<int>(),"プロジェクトファイル");
    */

    bpo::variables_map varmap;
    bpo::store(bpo::parse_command_line(argc, argv, opt), varmap);
    bpo::notify(varmap);
    if (varmap.count("help")) {
        std::cout << opt << std::endl;
        std::exit(EXIT_SUCCESS);
    }
    if (!varmap.count("project_file")) {
        spdlog::error("not enough arguments");
        std::cout << opt << std::endl;
        std::exit(EXIT_FAILURE);
    }
    if (varmap.count("debug")) {
        spdlog::set_level(spdlog::level::debug);
    }
    auto file_rootdir = varmap["chardef_dir"].as<std::string>();
    auto project_filepath = stdfsys::path(varmap["project_file"].as<std::string>());
    // auto dpmm = handfont::dpi_to_dpmm(varmap["dpi"].as<handfont::dpi>());
    // auto block_size = 1.0*dpmm;
    // for experiment
    // auto block_size = varmap["block_size"].as<int>();
    // auto constant = varmap["constant"].as<int>();
    // auto threshold = varmap["threshold"].as<int>();
    // auto diameter = varmap["diameter"].as<int>();
    /*
    auto sigma_color = varmap["sigma_color"].as<int>();
    auto sigma_space = varmap["sigma_space"].as<int>();
    */

    auto project_dir =
        project_filepath.parent_path();  // 画像の場所とかの相対パスの起点は、プロジェクトファイルの場所！
    for (const auto& filepath : stdfsys::directory_iterator(project_dir / "input")) {
        handfont::image_handler image(project_dir, file_rootdir, filepath.path());
        image.process();
    }
    return 0;
}
