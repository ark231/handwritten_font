#include<stdexcept>
#include<qrencode.h>
#include<string>
#include"input_pdf_generator.hpp"
#include"input_pdf_consts.hpp"
#include"parse_chardef.hpp"
namespace handfont{
	px mm_to_px(mm input_mm){
		return (input_mm*dpi)/mm_per_inch;
	}
	void error_handler (HPDF_STATUS error_no, HPDF_STATUS detail_no, void *user_data)
	{
		throw std::runtime_error("ERROR: error_no="+std::to_string((unsigned int)error_no)+", detail_no="+std::to_string((int)detail_no)); /* throw exception on error */
	}
	void input_pdf_generator::draw_write_grid(HPDF_Page& current_page,grid_size size,char_width width,guide_type type,px BL_x,px BL_y){
		px grid_height;
		px grid_width;
		switch(size){
			case grid_size::SMALL:
				grid_height=mm_to_px(height_grid::SMALL);
				break;
			case grid_size::LARGE:
				grid_height=mm_to_px(height_grid::LARGE);
				break;
		}
		switch(width){
			case char_width::HALF:
				grid_width=grid_height/2.0;
				break;
			case char_width::FULL:
				grid_width=grid_height;
				break;
		}
		HPDF_Page_GSave(current_page);
		HPDF_Page_SetRGBStroke(current_page,0.6,0.8,1.0);
		HPDF_Page_Rectangle(current_page,BL_x,BL_y,grid_width,grid_height);
		HPDF_Page_Stroke(current_page);
		HPDF_Page_SetLineWidth(current_page,0.5);
		switch(type){
			case guide_type::NONE:
				break;
			case guide_type::LATIN:
				HPDF_Page_MoveTo(current_page,BL_x,BL_y+(grid_height/3.0));
				HPDF_Page_LineTo(current_page,BL_x+grid_width,BL_y+(grid_height/3.0));
				HPDF_Page_MoveTo(current_page,BL_x,BL_y+2*(grid_height/3.0));
				HPDF_Page_LineTo(current_page,BL_x+grid_width,BL_y+2*(grid_height/3.0));
				HPDF_Page_Stroke(current_page);
				break;
			case guide_type::CROSS:
				HPDF_UINT16 dash_tmp[] ={3};
				HPDF_Page_SetDash(current_page,dash_tmp,1,1);
				HPDF_Page_MoveTo(current_page,BL_x,BL_y+(grid_height/2.0));
				HPDF_Page_LineTo(current_page,BL_x+grid_width,BL_y+(grid_height/2.0));
				HPDF_Page_MoveTo(current_page,BL_x+(grid_width/2.0),BL_y);
				HPDF_Page_LineTo(current_page,BL_x+(grid_width/2.0),BL_y+grid_height);
				HPDF_Page_Stroke(current_page);
				break;
		}
		//HPDF_Page_Stroke(current_page);
		HPDF_Page_GRestore(current_page);
	}
	void input_pdf_generator::draw_info_grid(HPDF_Page& current_page,grid_size size,char_width width,px BL_x,px BL_y,Unicode character){
		
	}
	HPDF_Image input_pdf_generator::generate_qr_code(const char* data){
		QRcode* qr_code;
		qr_code = QRcode_encodeString(data,0,QR_ECLEVEL_M,QR_MODE_8,1);
		if(qr_code == nullptr){
			throw std::runtime_error("error: failed to encode string to qrcode!");
		}
		int width = qr_code->width;
		uint8_t *pixels = new uint8_t[width*width];
		HPDF_Image result;
		for(int l=0;l<width*width;l++){
			pixels[l] = (~qr_code->data[l]&0x01)*0xff;
		}
		result = HPDF_LoadRawImageFromMem(pdf,pixels,width,width,HPDF_CS_DEVICE_GRAY,8);
		return result;
	}
	input_pdf_generator::input_pdf_generator(const std::string fontname){
		font_name=fontname;
		std::string qr_places[] = {"TL","TR","BL","BR"};
		for(int i = 1;i<4;i++){
			qr_codes[qr_places[i]] = generate_qr_code(qr_places[i].c_str());
		}
		pdf = HPDF_New(error_handler,nullptr);
		if(!pdf){
			throw std::runtime_error("error: failed to create pdf document!");
		}
	}
	void input_pdf_generator::add_page(chardef_filemeta filemeta){
		HPDF_Page current_page = HPDF_AddPage(pdf);
		pages.push_back(current_page);
		HPDF_Page_SetSize(current_page, HPDF_PAGE_SIZE_A4,HPDF_PAGE_PORTRAIT);
		chardef_data data;
		data.parse_chardef(filemeta);
		std::string qr_info_tmp="";
		qr_info_tmp = "TL_Handfont_"+filemeta.get_filecode()+"_"+font_name;
		qr_codes["TL"]= generate_qr_code(qr_info_tmp.c_str());
		draw_write_grid(current_page,data.size,char_width::HALF,guide_type::NONE,100,100);
		draw_write_grid(current_page,data.size,char_width::HALF,guide_type::LATIN,100+mm_to_px(height_grid::SMALL)/2.0,100);
		draw_write_grid(current_page,data.size,char_width::HALF,guide_type::CROSS,100+2*(mm_to_px(height_grid::SMALL)/2.0),100);
		draw_write_grid(current_page,data.size,char_width::FULL,guide_type::NONE,100+3*(mm_to_px(height_grid::SMALL)/2.0),100);
		draw_write_grid(current_page,data.size,char_width::FULL,guide_type::LATIN,100+5*(mm_to_px(height_grid::SMALL)/2.0),100);
		draw_write_grid(current_page,data.size,char_width::FULL,guide_type::CROSS,100+7*(mm_to_px(height_grid::SMALL)/2.0),100);
	}
	void input_pdf_generator::saveto_file(const std::string filename){
		HPDF_SaveToFile(pdf,filename.c_str());
	}
	input_pdf_generator::~input_pdf_generator(){
		HPDF_Free(pdf);
	}
}
