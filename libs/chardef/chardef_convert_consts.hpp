#ifndef CHARDEF_CONVERT_CONSTS
#define CHARDEF_CONVERT_CONSTS
#include "chardef_consts.hpp"
namespace handfont {
std::string to_string(const grid_size size);
std::string to_string(const font_type type);
std::string to_string(const guide_type);
grid_size to_grid_size(const std::string input);
font_type to_font_type(const std::string input);
}  // namespace handfont
#endif
