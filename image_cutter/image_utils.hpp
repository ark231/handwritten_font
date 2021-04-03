#ifndef HANDFONT_IMAGE_UTILS
#define HANDFONT_IMAGE_UTILS
#include<opencv2/core.hpp>
namespace handfont{
	template <class K,class V>
	struct key_value{
		K key;
		V value;

		key_value(K key,V value){
			this->key = key;
			this->value = value;
		}
	};
	template <class K,class V>
	bool operator<(const key_value<K,V>& one,const key_value<K,V>& other){
		return (one.key < other.key);
	}
	template <class P>
	struct side{
		P start;
		P end;
		side(P,P);
		double length();
	};

	using dpmm=double;
	using dpi=double;
	double constexpr inch_per_mm= 1/25.4;
	dpmm dpi_to_dpmm(dpi input_dpi);

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
		explicit operator globalPoint() const noexcept{
			return globalPoint(origin+point);
		};
		explicit operator cv::Point() const noexcept{
			return point;
		}
	};
}
#include"image_utils.cpp"
#endif
