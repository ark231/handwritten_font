#ifndef HANDFONT_IMAGE_POINTS
#define HANDFONT_IMAGE_POINTS
#include <opencv2/core.hpp>
namespace handfont {
class localPoint;
class globalPoint : public cv::Point {
    // cv::Point point;
   public:
    globalPoint(cv::Point);
    globalPoint() {}
    operator localPoint() = delete;
    /*
    operator cv::Point() const noexcept{
        return point;
    };
    */
};
template <class P>
bool Point_x_less(const P& one, const P& theother) {
    // return ((cv::Point)one).x < ((cv::Point)theother).x;
    return one.x < theother.x;
}
template <class P>
bool Point_y_less(const P& one, const P& theother) {
    // return ((cv::Point)one).y < ((cv::Point)theother).y;
    return one.y < theother.y;
}
class localPoint : public cv::Point {
    // cv::Point point;
    cv::Point origin;

   public:
    localPoint(cv::Point, cv::Point);
    localPoint(){};
    cv::Point get_origin() const noexcept;
    // cv::Point get_point() const noexcept;
    explicit operator globalPoint() const noexcept { return globalPoint(origin + cv::Point(x, y)); };
    /*
    explicit operator cv::Point() const noexcept{
        return point;
    }
    */
};
bool operator==(const localPoint&, const localPoint&);
}  // namespace handfont
#endif
