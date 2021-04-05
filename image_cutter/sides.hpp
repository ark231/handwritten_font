#ifndef HANDFONT_IMAGE_SIDES
#define HANDFONT_IMAGE_SIDES
namespace handfont{
	template <class P>
	struct side{
		P start;
		P end;
		side(P start,P end){
			this->start = start;
			this->end = end;
		}
		double length(){
			//return std::sqrt(std::pow(((cv::Point)end).x-((cv::Point)start).x,2)+std::pow(((cv::Point)end).y-((cv::Point)start).y,2));
			return std::sqrt(std::pow(end.x-start.x,2)+std::pow(end.y-start.y,2));
		}
	};
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
	bool operator<(side<P>& one,side<P>& theother){
		return one.length() < theother.length();
	}
}
#endif
