#include "chardef_convert_consts.hpp"

#include <iomanip>
#include <sstream>
#include <string>
namespace handfont {
std::string to_string(const grid_size size) {
    std::string result;
    switch (size) {
        case grid_size::SMALL:
            result = "small";
            break;
        case grid_size::LARGE:
            result = "large";
            break;
    }
    return result;
}
std::string to_string(const font_type type) {
    std::string result;
    switch (type) {
        case font_type::MONO:
            result = "mono";
            break;
        case font_type::PROPORTIONAL:
            result = "proportional";
            break;
    }
    return result;
}
std::string to_string(const guide_type type) {
    std::string result;
    switch (type) {
        case guide_type::NONE:
            result = "none";
            break;
        case guide_type::LATIN:
            result = "latin";
            break;
        case guide_type::CROSS:
            result = "cross";
            break;
        case guide_type::LOWER_LATIN:
            result = "lower_latin";
            break;
    }
    return result;
}
grid_size to_grid_size(const std::string input) {
    grid_size result;
    if (input == "small") {
        result = grid_size::SMALL;
    } else if (input == "large") {
        result = grid_size::LARGE;
    }
    return result;
}
font_type to_font_type(const std::string input) {
    font_type result;
    if (input == "mono") {
        result = font_type::MONO;
    } else if (input == "proportional") {
        result = font_type::PROPORTIONAL;
    }
    return result;
}
}  // namespace handfont
