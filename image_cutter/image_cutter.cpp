#include<opencv2/core.hpp>
#include<opencv2/imgproc.hpp>
#include<opencv2/imgcodecs.hpp>
#include<opencv2/highgui.hpp>
#include<opencv2/imgproc.hpp>
#include<opencv2/objdetect.hpp>
#include<boost/program_options.hpp>
#include<iostream>
#include<string>
#include<filesystem>
#include<vector>

#include"general/helpers.hpp"
namespace bpo=boost::program_options;
namespace stdfsys=std::filesystem;

int main(int argc,char *argv[]){
	bpo::options_description opt("option");
	opt.add_options()
		("help,h","show this help")
		("chardef_dir,d",bpo::value<std::string>()->default_value(handfont::get_self_dir().parent_path().native()+"/chardefs"),"文字定義ファイルのあるディレクトリ")
		("project_file,p",bpo::value<std::string>(),"プロジェクトファイル")
		//for experiment
		("block_size,s",bpo::value<int>(),"size of binarize block")
		("constant,c",bpo::value<int>(),"constant")
		("threshold,t",bpo::value<int>(),"threshold for edge detection")
		("diameter,a",bpo::value<int>(),"プロジェクトファイル");
		/*
		("sigma_color,b",bpo::value<int>(),"プロジェクトファイル")
		("sigma_space,e",bpo::value<int>(),"プロジェクトファイル");
		*/

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
	//for experiment
	auto block_size = varmap["block_size"].as<int>();
	auto constant = varmap["constant"].as<int>();
	auto threshold = varmap["threshold"].as<int>();
	auto diameter = varmap["diameter"].as<int>();
	/*
	auto sigma_color = varmap["sigma_color"].as<int>();
	auto sigma_space = varmap["sigma_space"].as<int>();
	*/

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
		cv::Mat image_bin(image.rows,image.cols,CV_8UC1);
		std::cout<<"start trying to binalize"<<std::endl;
		cv::threshold(image_blue,image_bin,0,0xff,cv::THRESH_BINARY|cv::THRESH_OTSU);
		//cv::adaptiveThreshold(image_blue,image_bin,0xff,cv::ADAPTIVE_THRESH_MEAN_C,cv::THRESH_BINARY,block_size,constant);
		std::cout<<"finish binalization"<<std::endl;
		cv::Mat image_blurred(image.rows,image.cols,CV_8UC1);
		//cv::bilateralFilter(image_bin,image_blurred,diameter,sigma_color,sigma_space);
		cv::blur(image_bin,image_blurred,cv::Size(diameter,diameter));
		cv::Mat image_edge(image.size(),CV_8UC1);

		//cv::Canny(image_blurred,image_edge,threshold,threshold*3,block_size);
		image_edge = image_blurred;
		auto image_edge2 = image_edge.clone();
		std::vector<std::vector<cv::Point>> contours;
		std::vector<cv::Vec4i> hierarchy;
		cv::findContours(image_edge,contours,hierarchy,cv::RETR_TREE,cv::CHAIN_APPROX_SIMPLE);
		cv::Mat result = cv::Mat::zeros(image_edge.size(),CV_8UC3);
		cv::RNG rng(12345);
		for( size_t i = 0; i< contours.size(); i++ ) {
			cv::Scalar color = cv::Scalar(rng.uniform(0, 256), rng.uniform(0,256), rng.uniform(0,256));
			cv::drawContours(result, contours, (int)i, color);
		}
		/*
		cv::QRCodeDetector qr_detector;
		std::vector<std::string> qr_infos;
		//std::vector<cv::Point> qr_vertices;
		cv::Mat qr_vertices;
		std::cout<<"start detecting and decoding qr codes"<<std::endl;
		qr_detector.detectAndDecodeMulti(image_bin,qr_infos,qr_vertices);
		if(qr_infos.empty()){
			std::cerr<<"error: couldn't detect nor decode qr codes in the input image!"<<std::endl;
			continue;
		*/
		auto outfilepath = (project_dir/"output"/(filepath.path().filename().native()+"_blue_edge.png"));
		try{
			cv::imwrite(outfilepath.native(),result);
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
