#include<cmath>
#include<stdexcept>
#include<opencv2/core.hpp>
#include"sides.hpp"
namespace handfont{
	template<class P>
	side<P>::side(P start,P end){
		this->start = start;
		this->end = end;
	}
	template<class P>
	double side<P>::length(){
		return std::sqrt(std::pow(((cv::Point)end).x-((cv::Point)start).x,2)+std::pow(((cv::Point)end).y-((cv::Point)start).y,2));
	}

	template <class P>
	P same_points(side<P> one,side<P> theother){
		if(one.start == theother.start){
			return one.start;
		}else if(one.start == theother.end){
			return one.start;
		}else if(one.end == theother.start){
			return one.end;
		}else if(one.end == theother.end){
			return one.end;
		}else{
			throw std::runtime_error("given two sides doesn't share any points");
		}
	}
	template <class P>
	bool operator <(side<P>& one,side<P>& theother){
		return one.length() < theother.length();
	}
}
