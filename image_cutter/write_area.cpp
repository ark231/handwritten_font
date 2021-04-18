#include<algorithm>
#include<array>
#include<string>
#include<map>
#include<opencv2/imgproc.hpp>
#include"write_area.hpp"
#include"image_utils.hpp"
namespace handfont{
	std::array<std::string,4> corner_names{"TL","TR","BR","BL"};
	std::map<std::string,corner> tagged_corners;
	write_area::write_area(std::vector<corner> corners){
		std::array<corner,4> ordered_corners;
		std::sort(corners.begin(),corners.end(),corner_y_less{});
		if(is_close(corners[0].point.y,corners[1].point.y,1*percent,close_type::REL)){
			if(Point_x_less(corners[0].point,corners[1].point)){//0が左
				ordered_corners[0] = corners[0];
				ordered_corners[1] = corners[1];
			}else{//0が右
				ordered_corners[0] = corners[1];
				ordered_corners[1] = corners[0];
			}
			if(Point_x_less(corners[2].point,corners[3].point)){//2が左
				ordered_corners[2] = corners[3];
				ordered_corners[3] = corners[2];
			}else{//2が右
				ordered_corners[2] = corners[2];
				ordered_corners[3] = corners[3];
			}
		}else{
			ordered_corners[0] = corners[0];
			if(Point_x_less(corners[1].point,corners[2].point)){//1が左
				ordered_corners[1] = corners[2];
				ordered_corners[3] = corners[1];
			}else{
				ordered_corners[1] = corners[1];
				ordered_corners[3] = corners[2];
			}
			ordered_corners[2] = corners[3];
		}
		int offset=-1;
		for(int i=0;i<2;i++){
			for(size_t j=0;j<4;j++){
				if(i==0 && ordered_corners[j].code.data.empty()){
					continue;
				}
				if(offset <0){
					auto idx_tmp = std::find(corner_names.begin(),corner_names.end(),ordered_corners[j].code.data.substr(0,2));
					offset = std::distance(corner_names.begin(),idx_tmp);
				}
				tagged_corners[corner_names[(j+offset)%4]] = ordered_corners[j];
			}
		}
		this->TL = tagged_corners["TL"];
		this->TR = tagged_corners["TR"];
		this->BR = tagged_corners["BR"];
		this->BL = tagged_corners["BL"];
	}
	bool write_area::is_valid(){
		if(!this->TL.code.data.empty() 
			&& TL.code.data.substr(0,2) == "TL"
			&& TL.code.data.length()>3
			){
			return true;
		}else if(!this->TR.code.data.empty() 
				&& TR.code.data.substr(0,2) == "TR"
				&& TR.code.data.length()>3
				){
			return true;
		}else if(!this->BR.code.data.empty() 
				&& BR.code.data.substr(0,2) == "BR"
				&& BR.code.data.length()>3
				){
			return true;
		}else if(!this->BL.code.data.empty() 
				&& BL.code.data.substr(0,2) == "BL"
				&& BL.code.data.length()>3
				){
			return true;
		}else{
			return false;
		}
	}
	std::string write_area::get_TL_data(){
		if(!TL.code.data.empty()){
			return TL.code.data;
		}else if(!TR.code.data.empty()){
			return TR.code.data;
		}else if(!BR.code.data.empty()){
			return BR.code.data;
		}else if(!BL.code.data.empty()){
			return BL.code.data;
		}else{
			return "";
		}
	}
	[[nodiscard]] cv::Mat write_area::centerize_image(cv::Mat& src){
		auto dest_width = cv::norm(TR.point-TL.point);
		auto dest_height = cv::norm(BR.point-TR.point);
		std::array<cv::Point2f,4> src_vertice{
			TL.point,
			TR.point,
			BR.point,
			BL.point
		};
		std::array<cv::Point2f,4> dest_vertice{
			cv::Point(0,0),
			cv::Point(dest_width,0),
			cv::Point(dest_width,dest_height),
			cv::Point(0,dest_height)
		};
		cv::Mat dest_tmp(src.size(),CV_8UC1);
		auto transform = cv::getPerspectiveTransform(src_vertice,dest_vertice);
		cv::warpPerspective(src,dest_tmp,transform,dest_tmp.size());
		return cv::Mat(dest_tmp,cv::Rect(0,0,dest_width,dest_height));
	}
		[[nodiscard]] std::map<std::string,corner> write_area::get_corners(){
			std::map<std::string,corner> result;
			result["TL"] = TL;
			result["TR"] = TR;
			result["BR"] = BR;
			result["BL"] = BL;
			return result;
		}
}
