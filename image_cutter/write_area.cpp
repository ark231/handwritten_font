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
		return !this->TL.code.data.empty();
	}
	std::string write_area::get_TL_data(){
		return TL.code.data;
	}
	void write_area::centerize_image(cv::Mat& res,cv::Mat& dest){
	}
}
