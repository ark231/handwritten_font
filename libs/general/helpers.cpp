#include<sstream>
#include<iomanip>
#include<string>

#include"helpers.hpp"
namespace handfont{
	std::string to_hex(int num){
		std::stringstream result;
		result<<std::hex<<num;
		return result.str();
	}
	std::string to_hex(int num,int width,const char fillchar){
		std::stringstream result;
		result<<std::setw(width)<<std::setfill(fillchar)<<std::hex<<num;
		return result.str();
	}
}
