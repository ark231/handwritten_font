#ifndef HANDFONT_IMAGE_CORNER
#define HANDFONT_IMAGE_CORNER
#include"qr_codes.hpp"
#include"points.hpp"
namespace handfont{
	struct corner{
		globalPoint point;
		qr_code code;
		corner(globalPoint,qr_code);
		corner();
	};
	struct corner_y_less{
		bool operator()(const corner& one,const corner& theother){
			return Point_y_less(one.point,theother.point);
		}
	};
}
#endif
