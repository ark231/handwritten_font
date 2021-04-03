#ifndef HANDFONT_IMAGE_POINTS
#define HANDFONT_IMAGE_POINTS
#include<opencv2/core.hpp>
namespace handfont{
	class localPoint;
	class globalPoint {
		cv::Point point;
		public:
		globalPoint(cv::Point);
		operator localPoint() = delete;
		operator cv::Point() const noexcept{
			return point;
		};
	};
	class localPoint{
		cv::Point point;
		cv::Point origin;
		public:
		localPoint(cv::Point,cv::Point);
		localPoint(){};
		cv::Point get_origin() const noexcept;
		cv::Point get_point() const noexcept;
		explicit operator globalPoint() const noexcept{
			return globalPoint(origin+point);
		};
		explicit operator cv::Point() const noexcept{
			return point;
		}
	};
	bool operator ==(const localPoint&,const localPoint&);
}
#endif
