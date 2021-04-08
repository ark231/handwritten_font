#include<opencv2/core.hpp>
#include<opencv2/imgproc.hpp>
#include<opencv2/imgcodecs.hpp>
//#include<opencv2/highgui.hpp>
#include<opencv2/imgproc.hpp>
#include<opencv2/objdetect.hpp>
#include<boost/program_options.hpp>
#include<iostream>
#include<string>
#include<filesystem>
#include<vector>
#include<spdlog/spdlog.h>
#include<algorithm>
#include<limits>
#include<regex>

#include"general/helpers.hpp"
#include"chardef/parse_chardef.hpp"
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
		("dpi,i",bpo::value<handfont::dpi>(),"入力画像のスケール")
		("debug,b","デバッグ用出力を有効化する");
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
		spdlog::error("not enough arguments");
		std::cout<<opt<<std::endl;
		std::exit(EXIT_FAILURE);
	}
	if(varmap.count("debug")){
		spdlog::set_level(spdlog::level::debug);
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
			spdlog::error("couldn't load input image file "+filepath.path().native());
			continue;
		}
		spdlog::info("successfully loaded input image "+filepath.path().native());
		cv::Mat image_blue(image.rows,image.cols,CV_8UC1);
		if(image.channels()!=1){
			cv::extractChannel(image,image_blue,0);
		}else{
			image_blue = image;
		}
		cv::Mat image_bin(image.rows,image.cols,CV_8UC1);
		spdlog::info("start trying to binalize");
		cv::threshold(image_blue,image_bin,0,0xff,cv::THRESH_BINARY|cv::THRESH_OTSU);
		//cv::adaptiveThreshold(image_blue,image_bin,0xff,cv::ADAPTIVE_THRESH_MEAN_C,cv::THRESH_BINARY,block_size,constant);
		spdlog::info("finish binalization");
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
		//cv::RNG rng(12345);
		/*
		for( size_t i = 0; i< contours.size(); i++ ) {
			cv::Scalar color = cv::Scalar(rng.uniform(0, 256), rng.uniform(0,256), rng.uniform(0,256));
			cv::drawContours(result, contours, (int)i, color);
		}
		*/
		cv::QRCodeDetector qr_detector;
		std::vector<std::string> qr_infos;
		std::vector<std::vector<cv::Point>> qr_vertices;// multi 'multi vertex'
		std::vector<handfont::qr_code> qr_codes;
		std::vector<std::vector<handfont::side<handfont::localPoint>>> markers_sides;
		spdlog::info("start detecting qr codes and corner marcers");
		int counter_corner=0;
		for(const auto& boundRect : boundRects){
			cv::Point origin(boundRect.x,boundRect.y);
			cv::Mat qr_area(image_bin,boundRect);
			cv::Mat qr_area_border(qr_area.rows+2*(5.0*dpmm),qr_area.cols+2*(5.0*dpmm),CV_8UC1);
			cv::copyMakeBorder(qr_area,qr_area_border,5.0*dpmm,5.0*dpmm,5.0*dpmm,5.0*dpmm,cv::BORDER_CONSTANT,0xff);
			handfont::qr_code code_tmp;
			std::vector<cv::Point> code_vertice;
			code_tmp.data = qr_detector.detectAndDecode(qr_area_border,code_vertice);
			if(code_vertice.size() == 4){
				std::vector<handfont::globalPoint> code_vertice_local;
				for(const auto& vertex:code_vertice){
					code_vertice_local.push_back((handfont::globalPoint)handfont::localPoint(vertex,origin));
				}
				code_tmp.vertice=code_vertice_local;
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

			//cv::Mat corner_edge = cv::Mat::zeros(corner_area_bin.size(),CV_8UC1);
			cv::Mat corner_edge(image_blue,boundRect);
			cv::Mat corner_edge_color;
			cv::cvtColor(corner_edge,corner_edge_color,cv::COLOR_GRAY2BGR);

			std::vector<std::vector<cv::Point>> corner_contours;
			std::vector<cv::Vec4i> hierarchy;
			cv::findContours(corner_area_bin,corner_contours,hierarchy,cv::RETR_TREE,cv::CHAIN_APPROX_SIMPLE);
			std::vector<std::vector<cv::Point>> contours_poly;
			contours_poly.reserve(contours.size());
			markers_sides.reserve(contours.size());
			for(const auto& contour:corner_contours){
				std::vector<cv::Point> contour_poly;
				cv::approxPolyDP( contour, contour_poly, cv::arcLength(contour,true)*(3/100.0), true );
				if(10*dpmm*10*dpmm<cv::contourArea(contour_poly)&&cv::contourArea(contour_poly)<corner_edge.rows*corner_edge.cols*(90/100.0)){
					if(contour_poly.size()==6){
						contours_poly.push_back(contour_poly);
						std::vector<handfont::side<handfont::localPoint>> marker_sides;
						for(int i=0;i<6;i++){
							handfont::localPoint start(contour_poly[i],origin);
							handfont::localPoint end(contour_poly[(i+1)%6],origin);
							marker_sides.push_back(handfont::side<handfont::localPoint>(start,end));
						}
						markers_sides.push_back(marker_sides);
					}
				}
			}
			spdlog::debug("{} contours found",std::to_string(contours_poly.size()));
			/*
			for(size_t i =0;i<contours_poly.size();i++){
				cv::Scalar color = cv::Scalar( rng.uniform(0, 256), rng.uniform(0,256), rng.uniform(0,256) );
				cv::drawContours( corner_edge_color, contours_poly, (int)i, color,2);
				int counter_vertex=0;
				for(const auto& vertex:contours_poly[i]){
					cv::drawMarker(corner_edge_color,vertex,color,cv::MARKER_CROSS,3*dpmm,2);
					cv::putText(corner_edge_color,std::to_string(counter_vertex),vertex,cv::FONT_HERSHEY_SIMPLEX,1,color);
					counter_vertex++;
				}
			}
			writeto_file((project_dir/"output"/(filepath.path().filename().native()+"_corner_edge"+std::to_string(counter_corner)+".png")),corner_edge_color);
			counter_corner++;
			*/
		}
		spdlog::debug("{} qr codes found",std::to_string(qr_codes.size()));
		spdlog::debug("{} markers found",std::to_string(markers_sides.size()));
		if(qr_codes.size() < 4){
			spdlog::error("couldn't detect nor decode enough qr codes in the input image!");
			continue;
		}
		if(markers_sides.size() < 4){
			spdlog::error("couldn't detect enough corner markers in the input image!");
			continue;
		}
		std::vector<handfont::corner> corners;
		for(auto& marker: markers_sides){
			//マーカーの辺を長さ順にソート
			std::sort(marker.begin(),marker.end());
			handfont::corner corner_tmp;
			//3番目、四番目に長い辺の共有点を取得
			try{
				corner_tmp.point = (handfont::globalPoint)handfont::same_points(marker[3-1],marker[4-1]);
			}
			catch(std::runtime_error excep){
				spdlog::error("couldn't find corner");
				spdlog::debug(excep.what());
				continue;
			}
			size_t closest_idx_qr;
			double current_dist=std::numeric_limits<double>::infinity();//無効である値かつどんな有効な値よりも「大きい」
			//最も近いqrコードを取得
			for(size_t idx_qr = 0;const auto& qr_code: qr_codes){
				double new_dist = cv::norm(qr_code.center()-(cv::Point)corner_tmp.point);
				if(current_dist > new_dist){//近い<=>距離が小さい!!!
					current_dist = new_dist;
					closest_idx_qr = idx_qr;
				}
				idx_qr++;
			}
			//確定したqrコードは候補から外す
			corner_tmp.code = qr_codes[closest_idx_qr];
			qr_codes.erase(qr_codes.begin()+closest_idx_qr);
			corners.push_back(corner_tmp);
		}
		handfont::write_area write_handler(corners);
		if(!write_handler.is_valid()){
			spdlog::error("couldn't decode TL qr code");
		}
		spdlog::debug(write_handler.get_TL_data());
		/*
#ifndef NDEBUG
		spdlog::debug("start drawing markers for debugging");
		cv::Mat result;
		cv::cvtColor(image_blue,result,cv::COLOR_GRAY2BGR);
		cv::RNG rng(12345);
		std::vector<std::string> names{"TL","TR","BR","BL"};
		auto name = names.begin();
		for(const auto& corner : write_handler.get_corners()){
			cv::Scalar color = cv::Scalar( rng.uniform(0, 256), rng.uniform(0,256), rng.uniform(0,256) );
			cv::drawMarker(result,corner.point,color,cv::MARKER_CROSS,3*dpmm,2);
			cv::putText(result,*name +":"+ corner.code.data.substr(0,2),corner.point,cv::FONT_HERSHEY_SIMPLEX,10,color);
			name++;
		}
		writeto_file((project_dir/"output"/(filepath.path().filename().native()+"_corners.png")),result);
#endif
*/
		auto image_write_area = write_handler.centerize_image(image_blue);
		/*
		writeto_file((project_dir/"output"/(filepath.path().filename().native()+"_write_area.png")),image_write_area);
		*/
		spdlog::info("start cutting off characters");
		std::smatch match_filecode;
		auto TL_data = write_handler.get_TL_data();
		std::regex_search(TL_data,match_filecode,std::regex("[SL][MP]_[0-9]+"));
		handfont::chardef_filemeta filemeta(match_filecode.str(),file_rootdir);
		handfont::chardef_data chardef;
		chardef.parse_chardef(filemeta);
		int num_internal_cols;
		int num_internal_rows;
		double write_grid_ratio;
		if(chardef.size == handfont::grid_size::SMALL){
			num_internal_cols=24*2;//48
			if(chardef.is_Fixed_Base){
				num_internal_rows=((30/2)/5)*4;//12
				write_grid_ratio=(3/5.0);
			}else{
				num_internal_rows=30/2;//15
				write_grid_ratio=(1/2.0);
			}
		}else if(chardef.size == handfont::grid_size::LARGE){
			num_internal_cols=24;
			if(chardef.is_Fixed_Base){
				num_internal_rows=((30-30%4)/4);//7
				write_grid_ratio=(3/4.0);
			}else{
				num_internal_rows=30/3;//10
				write_grid_ratio=(2/3.0);
			}
		}
		spdlog::debug("start making internal grids");
		double width_internal_grid = image_write_area.cols/(double)num_internal_cols;
		double height_internal_grid = image_write_area.rows/(double)num_internal_rows;
		std::vector<cv::Rect> internal_grids;
		internal_grids.reserve(num_internal_cols*num_internal_rows);
		for(size_t row=0;row < num_internal_rows;row++){
			for(size_t col=0;col < num_internal_cols;col++){
				internal_grids.push_back(cv::Rect(
							//width_internal_grid*col,
							(image_write_area.cols*col)/(double)num_internal_cols,
							//height_internal_grid*row,
							(image_write_area.rows*row)/(double)num_internal_rows,
							width_internal_grid,
							height_internal_grid
							));
			}
		}
		auto cache_dir = filemeta.dir_order(project_dir/".cache");
		if(!stdfsys::exists(cache_dir)){
			stdfsys::create_directories(cache_dir);
		}
		/*
		auto internal_grid = internal_grids.begin();
		for(int i=0;i<internal_grids.size();i++){
			writeto_file((cache_dir/(filepath.path().filename().native()+"_internal_grid_"+std::to_string(i)+".png")),cv::Mat(image_write_area,*internal_grid));
			internal_grid++;
		}
		*/
		spdlog::debug("start processing internal grids");
		double constexpr em = 1024;
		//double constexpr em_per_inch = (1.0*handfont::inch_per_mm)/(7.0/em);
		//double constexpr em_per_inch = (em/7.0)*handfont::mm_per_inch;
		double em_per_mm;
		if(chardef.size == handfont::grid_size::SMALL){
			em_per_mm = (em/7.0);
		}else if(chardef.size == handfont::grid_size::LARGE){
			em_per_mm = (em/14.0);
		}
		spdlog::debug("{}em/mm",em_per_mm);
		std::cout<<em_per_mm<<"em/mm"<<std::endl;
		auto internal_grid = internal_grids.begin();
		for(const auto& char_info : chardef.char_infos){
			cv::Rect unified_grid;
			if(char_info.width == handfont::char_width::HALF){
				unified_grid = *internal_grid;
				internal_grid++;
			}else if(char_info.width == handfont::char_width::FULL){
				if(std::distance(internal_grids.begin(),internal_grid)%num_internal_cols == num_internal_cols-1){
					internal_grid++;//端が余ったら飛ばす
				}
				unified_grid = *internal_grid | *(internal_grid+1);
				internal_grid += 2;
			}
			cv::Mat image_unified_grid(image_write_area,unified_grid);
			cv::Rect info_grid(0,0,unified_grid.width,unified_grid.height*(1-write_grid_ratio));
			cv::Rect write_grid(
					0,
					unified_grid.height*write_grid_ratio,
					unified_grid.width,
					unified_grid.height*write_grid_ratio
					);
			cv::Mat image_info_grid(image_unified_grid,info_grid);
			cv::Mat image_write_grid(image_unified_grid,write_grid);
			cv::Mat image_write_grid_resized(image_unified_grid,write_grid);
			cv::resize(image_write_grid,image_write_grid_resized,cv::Size(),(em_per_mm/dpmm),(em_per_mm/dpmm),cv::INTER_CUBIC);
			cv::Mat image_write_grid_bin(image_write_grid_resized.size(),CV_8UC1);
			cv::threshold(image_write_grid,image_write_grid_bin,0,0xff,cv::THRESH_BINARY|cv::THRESH_OTSU);
			//かすれ除去
			spdlog::debug("start unti-noise processes");
			cv::Mat image_write_grid_closed(image_write_area.rows,image_write_area.cols,CV_8UC1);
			auto element = cv::getStructuringElement(cv::MORPH_ELLIPSE,cv::Size(5,5));
			cv::morphologyEx(image_write_grid_bin,image_write_grid_closed,cv::MORPH_CLOSE,element);
			cv::Mat image_write_grid_opened(image_write_area.rows,image_write_area.cols,CV_8UC1);
			element = cv::getStructuringElement(cv::MORPH_ELLIPSE,cv::Size(10,10));
			cv::morphologyEx(image_write_grid_closed,image_write_grid_opened,cv::MORPH_OPEN,element);
			cv::Mat result = image_write_grid_opened;
			/* 
			 * 代替文字、不使用指定などを検知する処理を後で追加する 
			 */
			writeto_file((cache_dir/(handfont::to_hex(char_info.character,4,'0')+".png")),result);
		}
		/*
		for(const auto& qr_code: qr_codes){
			if(!qr_code.data.empty()){
				spdlog::debug(qr_code.data);
			}
		}
		*/
		//writeto_file((project_dir/"output"/(filepath.path().filename().native()+"_blue_edge.png")),result);

	}
	return 0;
}

void writeto_file(stdfsys::path outfilepath,cv::Mat image){
	try{
		cv::imwrite(outfilepath.native(),image);
	}
	catch(cv::Exception excep){
		spdlog::error("couldn't write image to "+outfilepath.native()+"\n{}",excep.what());
		std::exit(EXIT_FAILURE);
	}
	spdlog::info("successfully written into "+outfilepath.native());
}
