#include "corner.hpp"
namespace handfont {
corner::corner(globalPoint point, qr_code code) {
    this->point = point;
    this->code = code;
}
corner::corner() {}
/*
bool corner_y_less::operator()(const corner& one,const corner& theother){
    return Point_y_less(one.point,theother.point);
}
*/
}  // namespace handfont
