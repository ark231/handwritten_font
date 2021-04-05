#include"points.hpp"
namespace handfont{
	localPoint::localPoint(cv::Point point,cv::Point origin){
		//this->point = point;
		this->x = point.x;
		this->y = point.y;
		this->origin = origin;
	}
	/*
	cv::Point localPoint::get_point() const noexcept{
		return point;
	}
	*/
	cv::Point localPoint::get_origin() const noexcept{
		return origin;
	}
	bool operator==(const localPoint& one,const localPoint& theother){
		if(one.get_origin() != theother.get_origin()){
			return false;// 例外のほうがわかりやすいか？
		}else{
			return (one.x == theother.x) && (one.y == theother.y);
		}
	}
	globalPoint::globalPoint(cv::Point point){
		this->x = point.x;
		this->y = point.y;
	}
}
