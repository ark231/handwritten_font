#ifndef HANDFONT_GEN_INPUT_PDF
#define HANDFONT_GEN_INPUT_PDF
#include <hpdf.h>

#include <map>
#include <string>
#include <vector>

#include "chardef/parse_chardef.hpp"
#include "input_pdf_consts.hpp"
namespace handfont {
class input_pdf_generator {
    HPDF_Doc pdf;
    std::vector<HPDF_Page> pages;
    HPDF_Page current_page;
    std::string font_name;
    std::map<std::string, HPDF_Image> qr_codes;
    std::map<std::string, HPDF_Font>
        using_fonts;  // almost useless because I dont want to think how to switch fonts in cdef file
    std::string current_using_fontname;

    HPDF_Image generate_qr_code(const char*);
    mm draw_write_grid(grid_size, char_width, guide_type, bool, px, px);
    void draw_info_grid(grid_size, char_width, px, px, Unicode, bool);
    void draw_grid_set(grid_size, character_info, bool, px, px);
    void draw_corner_marker(px, px, corner_rotation);

   public:
    input_pdf_generator(const std::string);
    void add_page(chardef_filemeta);
    void saveto_file(const std::string);
    ~input_pdf_generator();
};
}  // namespace handfont
#endif
