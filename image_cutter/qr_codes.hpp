#ifndef HANDFONT_IMAGE_QR_CODES
#define HANDFONT_IMAGE_QR_CODES
#include<string>
#include<opencv2/core.hpp>
#include"points.hpp"
namespace handfont{
	struct qr_code{
		std::string data;
		std::vector<handfont::globalPoint> vertice;
		cv::Point center() const noexcept{
			cv::Point result(0,0);
			for(const auto& vertex:vertice){
				result.x+=vertex.x;
				result.y+=vertex.y;
			}
			result.x/=4.0;
			result.y/=4.0;
			return result;
		}
	};
}
#endif
