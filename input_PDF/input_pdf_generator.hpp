#ifndef HANDFONT_GEN_INPUT_PDF
#define HANDFONT_GEN_INPUT_PDF
#include<hpdf.h>
#include<vector>
#include<string>
#include<map>
#include"parse_chardef.hpp"
#include"input_pdf_consts.hpp"
namespace handfont{
	class input_pdf_generator{
		HPDF_Doc pdf;
		std::vector<HPDF_Page> pages;
		std::string font_name;
		std::map<std::string,HPDF_Image> qr_codes;
		HPDF_Image generate_qr_code(const char*);
		void draw_write_grid(HPDF_Page&,grid_size,char_width,guide_type,px,px);
		void draw_info_grid(HPDF_Page&,grid_size,char_width,px,px,Unicode);
		public:
		input_pdf_generator(const std::string);
		void add_page(chardef_filemeta);
		void saveto_file(const std::string);
		~input_pdf_generator();
	};
}
#endif
