#include<opencv2/core.hpp>
#include<opencv2/imgcodecs.hpp>
#include<opencv2/highgui.hpp>
#include<boost/program_options.hpp>
#include<iostream>
#include<string>
#include<filesystem>

#include"general/helpers.hpp"
namespace bpo=boost::program_options;
namespace stdfsys=std::filesystem;

int main(int argc,char *argv[]){
	bpo::options_description opt("option");
	opt.add_options()
		("help,h","show this help")
		("chardef_dir,d",bpo::value<std::string>()->default_value(handfont::get_self_dir().parent_path().native()+"/chardefs"),"文字定義ファイルのあるディレクトリ")
		("project_file,p",bpo::value<std::string>(),"プロジェクトファイル");
	bpo::variables_map varmap;
	bpo::store(bpo::parse_command_line(argc,argv,opt),varmap);
	bpo::notify(varmap);
	if(varmap.count("help")){
		std::cout<<opt<<std::endl;
		std::exit(EXIT_SUCCESS);
	}
	if(!varmap.count("project_file")){
		std::cout<<"error: not enough arguments"<<std::endl;
		std::cout<<opt<<std::endl;
		std::exit(EXIT_FAILURE);
	}
	auto file_rootdir = varmap["chardef_dir"].as<std::string>();
	auto project_filepath = stdfsys::path(varmap["project_file"].as<std::string>());
	auto project_dir = project_filepath.parent_path();
	for(const auto& filepath : stdfsys::directory_iterator(project_dir/"input")){
		auto image = cv::imread(filepath.path().native());
		if(image.empty()){
			std::cerr<<"error: couldn't load input image file "<<filepath.path().native()<<std::endl;
			continue;
		}
		std::cout<<"successfully loaded input image "<<filepath.path().native()<<std::endl;
		cv::Mat image_blue(image.rows,image.cols,CV_8UC1);
		if(image.channels()!=1){
			cv::extractChannel(image,image_blue,0);
		}else{
			image_blue = image;
		}
		auto outfilepath = (project_dir/"output"/(filepath.path().filename().native()+"_blue.png"));
		try{
			cv::imwrite(outfilepath.native(),image_blue);
		}
		catch(cv::Exception excep){
			std::cerr<<"error: couldn't write image to "<<outfilepath.native()<<std::endl;
			std::cerr<<excep.what()<<std::endl;
			std::exit(EXIT_FAILURE);
		}
		std::cout<<"successfully written into "<<outfilepath.native()<<std::endl;
	}
	return 0;
}
