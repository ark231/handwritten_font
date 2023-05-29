#include "parse_chardef.hpp"

#include <fstream>
#include <iostream>
#include <regex>
#include <stdexcept>
#include <string>
namespace handfont {
void other_char::set_all(Unicode character, FileCode code, ChardefDirmeta rootdir) {
    filemeta = chardef_filemeta(code, rootdir);
    this->character = character;
}
std::vector<std::string> parse_list(std::string input) {
    std::vector<std::string> result;
    int endof_list;
    if ((endof_list = input.find("]", 0)) == std::string::npos) {
        throw std::runtime_error("error: list is not closed!");
    }
    std::string list_all_raw = input.substr(1, endof_list - 1);
    int start_val = 0;
    int place_comma;
    while ((place_comma = list_all_raw.find(",", start_val)) != std::string::npos) {
        result.push_back(list_all_raw.substr(start_val, place_comma - start_val));
        start_val += place_comma - start_val + 1;
    }
    result.push_back(list_all_raw.substr(start_val));  // there are no comma after the last value
    return result;
}
void chardef_data::parse_chardef(chardef_filemeta input_filemeta) {
    auto chardef_file = std::ifstream(input_filemeta.path());
    if (chardef_file.fail()) {
        throw std::runtime_error("error: could not open chardef file: " + input_filemeta.path());
    }
    is_Fixed_Base = false;  // initialization and default value
    std::string line;
    while (std::getline(chardef_file, line)) {
        if (line[0] == '#' || line.empty()) {  // if the line obviously seems to contain no data, then skip it!
            continue;
        }
        std::string uncommented_line;
        if (line.find("#", 0) == std::string::npos) {
            uncommented_line = line;  // the line doesn't contain any comment, so it is already "uncommented"
        } else {
            uncommented_line = line.substr(0, line.find("#", 0));
        }
        bool there_is_keyvalue = false;  // initialization
        std::string name = "";
        std::string raw_data = "";
        int place_dollar = uncommented_line.find("$");
        if (place_dollar != std::string::npos) {  // there is a key-value data
            name =
                uncommented_line.substr(place_dollar + 1, uncommented_line.find(" ", place_dollar) - place_dollar - 1);
            raw_data = uncommented_line.substr(uncommented_line.find("=", 0) + 2);  //+2 means "= "
            there_is_keyvalue = true;
        }
        if (there_is_keyvalue && uncommented_line.find("$") == 0) {  // metadata
            if (name == "Langs" || name == "LANGS") {                // character type list
                char_typenames = parse_list(raw_data);
            } else if (name == "Fonts") {
                required_fontnames = parse_list(raw_data);
            } else if (name == "Type" || name == "TYPE") {
                if (raw_data == "Mono") {
                    f_type = font_type::MONO;
                } else if (raw_data == "Proportional") {
                    f_type = font_type::PROPORTIONAL;
                }
            } else if (name == "Size" || name == "SIZE") {
                if (uncommented_line.find("small", 0) != std::string::npos) {
                    size = grid_size::SMALL;
                } else if (uncommented_line.find("large", 0) != std::string::npos) {
                    size = grid_size::LARGE;
                } else {
                    throw std::runtime_error("error: invalid grid size \"" + uncommented_line + "\"");
                }
            } else if (name == "Fixed_Base" && raw_data.find("true") != std::string::npos) {
                is_Fixed_Base = true;
            }
        }
        if (uncommented_line[0] == 'H' || uncommented_line[0] == 'F') {  // chardef line!!
            character_info char_tmp;
            if (there_is_keyvalue) {
                uncommented_line = uncommented_line.substr(0, uncommented_line.find("$", 0));  // remove keyvalue data
            }
            switch (uncommented_line[0]) {
                case 'H':
                    char_tmp.width = char_width::HALF;
                    break;
                case 'F':
                    char_tmp.width = char_width::FULL;
                    break;
            }
            switch (uncommented_line[1]) {
                case 'n':
                    char_tmp.g_type = guide_type::NONE;
                    break;
                case 'l':
                    char_tmp.g_type = guide_type::LOWER_LATIN;
                    break;
                case 'L':
                    char_tmp.g_type = guide_type::LATIN;
                    break;
                case 'c':
                    char_tmp.g_type = guide_type::CROSS;
                    break;
            }
            char_tmp.character = (Unicode)std::stoi(uncommented_line.substr(4), nullptr, 16);
            char_tmp.has_alternative = false;  // initialization and default value
            if (there_is_keyvalue) {
                if (name == "Instead") {
                    char_tmp.has_alternative = true;
                    other_char alt_tmp;
                    int first_underline = raw_data.find("_");
                    int second_underline = raw_data.find("_", first_underline + 1);
                    std::string filecode = raw_data.substr(0, second_underline);
                    Unicode alt_char_tmp = (Unicode)std::stoi(raw_data.substr(second_underline + 2), nullptr, 16);
                    alt_tmp.set_all(alt_char_tmp, filecode, input_filemeta.rootdir());
                    char_tmp.alternative = alt_tmp;
                }
            }
            char_infos.push_back(char_tmp);
        }
    }
}
}  // namespace handfont
