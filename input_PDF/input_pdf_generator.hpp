#ifndef HANDFONT_GEN_INPUT_PDF
#define HANDFONT_GEN_INPUT_PDF
#include<hpdf.h>
#include<vector>
#include<string>
#include<map>
#include"chardef/parse_chardef.hpp"
#include"input_pdf_consts.hpp"
namespace handfont{
	class input_pdf_generator{
		HPDF_Doc pdf;
		std::vector<HPDF_Page> pages;
		std::string font_name;
		std::map<std::string,HPDF_Image> qr_codes;
		HPDF_Image generate_qr_code(const char*);
		mm draw_write_grid(HPDF_Page&,grid_size,char_width,guide_type,bool,px,px);
		std::map<std::string,HPDF_Font> using_fonts;//almost useless because I dont want to think how to switch fonts in cdef file
		std::string current_using_fontname;
		void draw_info_grid(HPDF_Page&,grid_size,char_width,px,px,Unicode);
		void draw_grid_set(HPDF_Page&,grid_size,character_info,bool,px,px);
		public:
		input_pdf_generator(const std::string);
		void add_page(chardef_filemeta);
		void saveto_file(const std::string);
		~input_pdf_generator();
	};
}
#endif
