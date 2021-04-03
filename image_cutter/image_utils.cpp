#include"image_utils.hpp"
#include<cmath>

namespace handfont{
	dpmm dpi_to_dpmm(dpi input_dpi){
		return (input_dpi*inch_per_mm);
	}
	template<class P>
	side<P>::side(P start,P end){
		this->start = start;
		this->end = end;
	}
	template<class P>
	double side<P>::length(){
		return std::sqrt(std::pow(((cv::Point)end).x-((cv::Point)start).x,2)+std::pow(((cv::Point)end).y-((cv::Point)start).y,2));
	}
	localPoint::localPoint(cv::Point point,cv::Point origin){
		this->point = point;
		this->origin = origin;
	}
	globalPoint::globalPoint(cv::Point point){
		this->point = point;
	}
}
