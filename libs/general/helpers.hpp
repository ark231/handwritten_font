#ifndef HANDFONT_HELPERS
#define HANDFONT_HELPERS
#include<string>
#include"chardef/chardef_consts.hpp"
namespace handfont{
	std::string to_hex(int);
	std::string to_hex(int,int,const char);
	std::string to_string(const grid_size);
	std::string to_string(const font_type);
	grid_size to_grid_size(const std::string);
	font_type to_font_type(const std::string);
}
#endif
