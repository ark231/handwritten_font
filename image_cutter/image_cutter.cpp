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

struct qr_code{
	std::string data;
	std::vector<cv::Point> vertice;
};

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

		//cv::Mat result = cv::Mat::zeros(image_edge.size(),CV_8UC3);
		cv::RNG rng(12345);
		/*
		for( size_t i = 0; i< contours.size(); i++ ) {
			cv::Scalar color = cv::Scalar(rng.uniform(0, 256), rng.uniform(0,256), rng.uniform(0,256));
			cv::drawContours(result, contours, (int)i, color);
		}
		*/
		cv::QRCodeDetector qr_detector;
		std::vector<std::string> qr_infos;
		std::vector<std::vector<cv::Point>> qr_vertices;// multi 'multi vertex'
		std::vector<qr_code> qr_codes;
		std::cout<<"start detecting qr codes and corner marcers"<<std::endl;
		int counter_corner=0;
		for(const auto& boundRect : boundRects){
			cv::Mat qr_area(image_bin,boundRect);
			cv::Mat qr_area_border(qr_area.rows+2*(5.0*dpmm),qr_area.cols+2*(5.0*dpmm),CV_8UC1);
			cv::copyMakeBorder(qr_area,qr_area_border,5.0*dpmm,5.0*dpmm,5.0*dpmm,5.0*dpmm,cv::BORDER_CONSTANT,0xff);
			qr_code code_tmp;
			code_tmp.data = qr_detector.detectAndDecode(qr_area_border,code_tmp.vertice);
			if(code_tmp.vertice.size() == 4){
				qr_codes.push_back(code_tmp);
				continue;
			}
			/*check if a corner marcer is in the area*/
			cv::Mat corner_area(image_blue,boundRect);
			cv::Mat corner_area_bin(corner_area.size(),CV_8UC1);
			/*
			double min_val;
			cv::minMaxLoc(corner_area,&min_val);
			std::cout<<std::to_string(min_val)<<" "<<std::to_string(min_val*(1+20/100.0))<<std::endl;
			*/
			//cv::threshold(corner_area,corner_area_bin,min_val*(1+20/100.0),0xff,cv::THRESH_BINARY);
			cv::threshold(corner_area,corner_area_bin,0,0xff,cv::THRESH_BINARY|cv::THRESH_OTSU);
			/*
			writeto_file((project_dir/"output"/(filepath.path().filename().native()+"_corner_raw"+std::to_string(counter_corner)+".png")),corner_area);
			writeto_file((project_dir/"output"/(filepath.path().filename().native()+"_corner"+std::to_string(counter_corner)+".png")),corner_area_bin);
			*/
			cv::Mat corner_edge = cv::Mat::zeros(corner_area_bin.size(),CV_8UC1);
			std::vector<std::vector<cv::Point>> corner_contours;
			std::vector<cv::Vec4i> hierarchy;
			cv::findContours(corner_area_bin,corner_contours,hierarchy,cv::RETR_TREE,cv::CHAIN_APPROX_SIMPLE);
			std::vector<std::vector<cv::Point>> contours_poly;
			contours_poly.reserve(contours.size());
			for(const auto& contour:corner_contours){
				std::vector<cv::Point> contour_poly;
				cv::approxPolyDP( contour, contour_poly, cv::arcLength(contour,true)*(3/100.0), true );
				if(10*dpmm*10*dpmm<cv::contourArea(contour_poly)&&cv::contourArea(contour_poly)<corner_edge.rows*corner_edge.cols*(90/100.0)){
					if(contour_poly.size()==5){
						contours_poly.push_back(contour_poly);
					}
				}
			}
			std::cout<<std::to_string(contours_poly.size())<<std::endl;
			for(size_t i =0;i<contours_poly.size();i++){
				cv::Scalar color = cv::Scalar( rng.uniform(0, 256), rng.uniform(0,256), rng.uniform(0,256) );
				cv::drawContours( corner_edge, contours_poly, (int)i, color,2);
				int counter_vertex=0;
				for(const auto& vertex:contours_poly[i]){
					cv::drawMarker(corner_edge,vertex,color,cv::MARKER_CROSS,3*dpmm,2);
					cv::putText(corner_edge,std::to_string(counter_vertex),vertex,cv::FONT_HERSHEY_SIMPLEX,1,color);
					counter_vertex++;
				}
			}
			writeto_file((project_dir/"output"/(filepath.path().filename().native()+"_corner_edge"+std::to_string(counter_corner)+".png")),corner_edge);
			counter_corner++;
		}
		std::cout<<std::to_string(qr_codes.size())<<std::endl;
		if(qr_codes.size() < 4){
			std::cerr<<"error: couldn't detect nor decode enough qr codes in the input image!"<<std::endl;
			continue;
		}
		for(const auto& qr_code: qr_codes){
			if(!qr_code.data.empty()){
				std::cout<<qr_code.data<<std::endl;
			}
		}
		//writeto_file((project_dir/"output"/(filepath.path().filename().native()+"_blue_edge.png")),result);

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
