#include<sstream>
#include<iomanip>
#include<string>

#include"helpers.hpp"
#include"chardef/chardef_consts.hpp"
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
	std::string to_string(const grid_size size){
		std::string result;
		switch(size){
			case grid_size::SMALL:
				result = "small";
				break;
			case grid_size::LARGE:
				result = "large";
				break;
		}
		return result;
	}
	std::string to_string(const font_type type){
		std::string result;
		switch(type){
			case font_type::MONO:
				result = "mono";
				break;
			case font_type::PROPORTIONAL:
				result = "proportional";
				break;
		}
		return result;
	}
	grid_size to_grid_size(const std::string input){
		grid_size result;
		if(input == "small"){
			result = grid_size::SMALL;
		}else if(input == "large"){
			result = grid_size::LARGE;
		}
		return result;
	}
	font_type to_font_type(const std::string input){
		font_type result;
		if(input == "mono"){
			result = font_type::MONO;
		}else if(input == "proportional"){
			result = font_type::PROPORTIONAL;
		}
		return result;
	}
}
