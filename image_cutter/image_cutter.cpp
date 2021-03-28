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
#include"image_utils.hpp"
namespace bpo=boost::program_options;
namespace stdfsys=std::filesystem;

void writeto_file(stdfsys::path outfilepath,cv::Mat);

int main(int argc,char *argv[]){
	bpo::options_description opt("option");
	opt.add_options()
		("help,h","show this help")
		("chardef_dir,d",bpo::value<std::string>()->default_value(handfont::get_self_dir().parent_path().native()+"/chardefs"),"文字定義ファイルのあるディレクトリ")
		("project_file,p",bpo::value<std::string>(),"プロジェクトファイル")
		("dpi,i",bpo::value<handfont::dpi>(),"入力画像のスケール");
		//for experiment
		//("block_size,s",bpo::value<int>(),"size of binarize block")
		//("constant,c",bpo::value<int>(),"constant")
		//("threshold,t",bpo::value<int>(),"threshold for edge detection")
		//("diameter,a",bpo::value<int>(),"size of blur kernel");
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
	auto dpmm = handfont::dpi_to_dpmm(varmap["dpi"].as<handfont::dpi>());
	auto block_size = 1.0*dpmm;
	//for experiment
	//auto block_size = varmap["block_size"].as<int>();
	//auto constant = varmap["constant"].as<int>();
	//auto threshold = varmap["threshold"].as<int>();
	//auto diameter = varmap["diameter"].as<int>();
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
		//cv::blur(image_bin,image_blurred,cv::Size(diameter,diameter));
		image_blurred = image_bin;

		cv::Mat image_dialate(image.size(),CV_8UC1);
		cv::Mat element = cv::getStructuringElement( cv::MORPH_RECT,cv::Size(block_size,block_size));
		cv::erode( image_blurred, image_dialate, element );
		/*
		auto outfilepath_erode = (project_dir/"output"/(filepath.path().filename().native()+"_blue_erode.png"));
		try{
			cv::imwrite(outfilepath_erode.native(),image_dialate);
		}
		catch(cv::Exception excep){
			std::cerr<<"error: couldn't write image to "<<outfilepath_erode.native()<<std::endl;
			std::cerr<<excep.what()<<std::endl;
			std::exit(EXIT_FAILURE);
		}
		std::cout<<"successfully written into "<<outfilepath_erode.native()<<std::endl;
		*/

		cv::Mat image_edge(image.size(),CV_8UC1);

		//cv::Canny(image_blurred,image_edge,threshold,threshold*3,block_size);
		image_edge = image_dialate;
		auto image_edge2 = image_edge.clone();
		std::vector<std::vector<cv::Point>> contours;
		std::vector<cv::Vec4i> hierarchy;
		cv::findContours(image_edge,contours,hierarchy,cv::RETR_TREE,cv::CHAIN_APPROX_SIMPLE);

		std::vector<std::vector<cv::Point>> contours_poly(contours.size());
		std::vector<cv::Rect> boundRects;
		for( size_t i = 0; i < contours.size(); i++ ){
			//cv::approxPolyDP( contours[i], contours_poly[i], 3, true );
			contours_poly[i] = contours[i];
			auto bound_rect_tmp = cv::boundingRect(contours_poly[i]);
			/*ignore too small or too big rectangles*/
			if(15*dpmm*15*dpmm < bound_rect_tmp.area() && bound_rect_tmp.area() < image_blue.rows*image_blue.cols*(90/100.0)){
				boundRects.push_back(bound_rect_tmp);
			}
		}

		cv::Mat result = cv::Mat::zeros(image_edge.size(),CV_8UC3);
		cv::RNG rng(12345);
		for( size_t i = 0; i< contours.size(); i++ ) {
			cv::Scalar color = cv::Scalar(rng.uniform(0, 256), rng.uniform(0,256), rng.uniform(0,256));
			cv::drawContours(result, contours, (int)i, color);
			//cv::rectangle(result,boundRects[i].tl(),boundRects[i].br(),color,3);
		}
		cv::QRCodeDetector qr_detector;
		std::vector<std::string> qr_infos;
		std::vector<std::vector<cv::Point>> qr_vertices;// multi 'multi vertex'
		//int counta_qr_area=0;
		std::cout<<"start detecting qr codes"<<std::endl;
		for(const auto& boundRect : boundRects){
			/*
			cv::Scalar color = cv::Scalar(rng.uniform(0, 256), rng.uniform(0,256), rng.uniform(0,256));
			cv::rectangle(result,boundRect.tl(),boundRect.br(),color,3);
			*/
			//cv::Mat qr_area(image_blue,boundRect);
			cv::Mat qr_area(image_bin,boundRect);
			cv::Mat qr_area_border(qr_area.rows+2*(5.0*dpmm),qr_area.cols+2*(5.0*dpmm),CV_8UC1);
			cv::copyMakeBorder(qr_area,qr_area_border,5.0*dpmm,5.0*dpmm,5.0*dpmm,5.0*dpmm,cv::BORDER_CONSTANT,0xff);
			std::vector<cv::Point> qr_vertice;
			qr_infos.push_back(qr_detector.detectAndDecode(qr_area_border,qr_vertice));
			qr_vertices.push_back(qr_vertice);
			/*
			writeto_file((project_dir/"output"/(filepath.path().filename().native()+"_blue_qr"+std::to_string(counta_qr_area)+".png")),qr_area_border);
			counta_qr_area++;
			*/
		}
		int num_qr_vertice=0;
		for(const auto& qr_vertice : qr_vertices){
			num_qr_vertice+=qr_vertice.size();
		}
		std::cout<<std::to_string(num_qr_vertice)<<std::endl;
		//if(qr_vertices.size() < 4*4){
		if(num_qr_vertice < 4*4){
			std::cerr<<"error: couldn't detect nor decode enough qr codes in the input image!"<<std::endl;
			continue;
		}
		for(const auto& qr_info : qr_infos){
			if(!qr_info.empty()){
				std::cout<<qr_info<<std::endl;
			}
		}
		/*
		cv::Mat qr_vertices;
		std::cout<<"start detecting and decoding qr codes"<<std::endl;
		qr_detector.detectAndDecodeMulti(image_bin,qr_infos,qr_vertices);
		if(qr_infos.empty()){
			std::cerr<<"error: couldn't detect nor decode qr codes in the input image!"<<std::endl;
			continue;
		*/
		writeto_file((project_dir/"output"/(filepath.path().filename().native()+"_blue_edge.png")),result);

	}
	return 0;
}

void writeto_file(stdfsys::path outfilepath,cv::Mat image){
	try{
		cv::imwrite(outfilepath.native(),image);
	}
	catch(cv::Exception excep){
		std::cerr<<"error: couldn't write image to "<<outfilepath.native()<<std::endl;
		std::cerr<<excep.what()<<std::endl;
		std::exit(EXIT_FAILURE);
	}
	std::cout<<"successfully written into "<<outfilepath.native()<<std::endl;
}
