#ifndef HANDFONT_IMAGE_WRITE_AREA
#define HANDFONT_IMAGE_WRITE_AREA
#include<opencv2/core.hpp>
#include<vector>
#include<map>
#include"corner.hpp"
namespace handfont{
	class write_area{
		corner TL;
		corner TR;
		corner BR;
		corner BL;
		public:
		write_area(std::vector<corner>);
		write_area(){}
		bool is_valid();
		std::string get_TL_data();
		[[nodiscard]] cv::Mat centerize_image(cv::Mat&);
		[[nodiscard]] std::map<std::string,corner> get_corners();
	};
}
#endif
