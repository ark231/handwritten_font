#include<hpdf.h>
#include<boost/program_options.hpp>
#include<string>
#include<iostream>
#include<map>
#include<vector>
#include<filesystem>
#include"parse_chardef.hpp"
#include"chardef_filemeta.hpp"
#include"input_pdf_generator.hpp"
#include"input_pdf_consts.hpp"
int main(int argc,char *argv[]){
	namespace bpo=boost::program_options;
	namespace stdfsys=std::filesystem;
	bpo::options_description opt("option");
	opt.add_options()
		("help,h","show this help")
		("grid_size,g",bpo::value<std::string>(),"枠のサイズ 必須")
		("font_type,f",bpo::value<std::string>(),"フォントのタイプ 必須")
		("file_id,i",bpo::value<std::string>(),"ファイルid 必須")
		("chardef_dir,d",bpo::value<std::string>()->default_value("../chardefs"),"文字定義ファイルのあるディレクトリ");
	bpo::variables_map varmap;
	bpo::store(bpo::parse_command_line(argc,argv,opt),varmap);
	bpo::notify(varmap);
	if(varmap.count("help")){
		std::cout<<opt<<std::endl;
		std::exit(EXIT_SUCCESS);
	}
	if(!varmap.count("grid_size") || !varmap.count("font_type") || !varmap.count("file_id")){
		std::cout<<"error: not enough arguments"<<std::endl;
		std::cout<<opt<<std::endl;
		std::exit(EXIT_FAILURE);
	}
	auto grid_size = varmap["grid_size"].as<std::string>();
	auto font_type = varmap["font_type"].as<std::string>();
	auto file_id = varmap["file_id"].as<std::string>();
	auto file_rootdir = varmap["chardef_dir"].as<std::string>();
	handfont::filecode input_code;
	if(grid_size == "small"){
		input_code+="S";
	}else if(grid_size == "large"){
		input_code+="L";
	}else{
		std::cerr<<"error: unknown grid size: '"+grid_size+"'"<<std::endl;
	}
	if(font_type == "mono"){
		input_code+="M";
	}else if(font_type == "proportional"){
		input_code+="P";
	}else{
		std::cerr<<"error: unknown font type: '"+font_type+"'"<<std::endl;
	}
	input_code+="_"+file_id;
	handfont::chardef_data data;
	handfont::chardef_filemeta input_filemeta(input_code,file_rootdir);
	handfont::input_pdf_generator result("testfont");
	result.add_page(input_filemeta);
	stdfsys::path outpath="./test.pdf";
	result.saveto_file(outpath.native());
	return 0;
}
