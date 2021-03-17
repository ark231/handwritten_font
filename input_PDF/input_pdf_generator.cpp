#include<stdexcept>
#include<qrencode.h>
#include<string>
#include<unicode/unistr.h>
#include<cmath>
#include<iostream>
#include<execinfo.h>
#include<cxxabi.h>
#include<regex>
#include<filesystem>
#include"input_pdf_generator.hpp"
#include"input_pdf_consts.hpp"
#include"chardef/parse_chardef.hpp"
#include"general/helpers.hpp"
namespace stdfsys = std::filesystem;
namespace handfont{
	px constexpr mm_to_px(mm input_mm){
		return (input_mm*dpi)/mm_per_inch;
	}
	void error_handler (HPDF_STATUS error_no, HPDF_STATUS detail_no, void *user_data)
	{
		constexpr int trace_size = 10;
		void *trace[trace_size];
		auto size = backtrace(trace, sizeof(trace));
		auto symbols = backtrace_symbols(trace,size);
		std::string err_msg = "ERROR: error_no=0x"+to_hex((unsigned int)error_no)+", detail_no=0x"+to_hex((int)detail_no);
		err_msg +="\n\nBacktrace:";
		for(int i = 0;i<size;i++){
			std::smatch funcname;
			auto symbol = std::string(symbols[i]);
			auto getfuncname_regex = std::regex(R"(.*\((.+)\+.+\))");
			std::regex_search(symbol,funcname,getfuncname_regex);
			int status;
			auto real_funcname = abi::__cxa_demangle(funcname[1].str().c_str(),0,0,&status);
			std::string demangled_symbol;
			if(real_funcname == nullptr){
				demangled_symbol = symbol;
			}else{
				auto replace_funcname_regex = std::regex(R"((.*)\(.+(\+.+)\)(.*))");
				auto format_replace = "$1("+std::string(real_funcname)+"$2)$3";
				demangled_symbol = std::regex_replace(symbol,replace_funcname_regex,format_replace);
			}
			err_msg += "\n"+demangled_symbol;
		}
		delete symbols;
		throw std::runtime_error(err_msg); /* throw exception on error */
	}

	mm input_pdf_generator::draw_write_grid(grid_size size,char_width width,guide_type type,bool is_Fixed_Based,px BL_x,px BL_y){
		px grid_height;
		mm result_height;
		px grid_width;
		px y_offset=0;
		switch(size){
			case grid_size::SMALL:
				grid_height=mm_to_px(height_grid::SMALL);
				result_height=height_grid::SMALL;
				break;
			case grid_size::LARGE:
				grid_height=mm_to_px(height_grid::LARGE);
				result_height=height_grid::LARGE;
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
		if(is_Fixed_Based){
			grid_height*=3/2.0;
			result_height*=3/2.0;
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
			case guide_type::LOWER_LATIN:
				HPDF_Page_MoveTo(current_page,BL_x,BL_y+(grid_height/4.0));
				HPDF_Page_LineTo(current_page,BL_x+grid_width,BL_y+(grid_height/4.0));
				HPDF_Page_MoveTo(current_page,BL_x,BL_y+5*(grid_height/8.0));
				HPDF_Page_LineTo(current_page,BL_x+grid_width,BL_y+5*(grid_height/8.0));
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
		return result_height;
	}

	void input_pdf_generator::draw_info_grid(grid_size size,char_width width,px BL_x,px BL_y,Unicode character,bool has_alter){
		px grid_height=mm_to_px(height_grid::SMALL);
		px grid_width;
		switch(size){
			case grid_size::SMALL:
				grid_width=mm_to_px(height_grid::SMALL);
				break;
			case grid_size::LARGE:
				grid_width=mm_to_px(height_grid::LARGE);
				break;
		}
		switch(width){
			case char_width::HALF:
				grid_width*=1/2.0;
				break;
			case char_width::FULL:
				//grid_width*=1;
				break;
		}
		HPDF_Page_GSave(current_page);
		HPDF_Page_SetRGBStroke(current_page,0.6,0.8,1.0);
		if(has_alter){
			HPDF_Page_SetRGBFill(current_page,0.9,1.0,1.0);
			HPDF_Page_Rectangle(current_page,BL_x,BL_y,grid_width,grid_height);
			HPDF_Page_FillStroke(current_page);
		}else{
			HPDF_Page_Rectangle(current_page,BL_x,BL_y,grid_width,grid_height);
			HPDF_Page_Stroke(current_page);
		}
		HPDF_Page_GRestore(current_page);
		icu::UnicodeString unistr((UChar32)character);
		std::string char_to_show;
		unistr.toUTF8String(char_to_show);
		HPDF_Page_BeginText(current_page);
		auto char_width = HPDF_Font_GetUnicodeWidth(using_fonts[current_using_fontname],character)*(mm_to_px(font_size)/1000.0);
		auto char_xoffset = (grid_width-char_width)/2.0;
		HPDF_Page_TextOut(current_page,BL_x+char_xoffset,BL_y+mm_to_px(font_size/5.0)+1,char_to_show.c_str());
		HPDF_Page_EndText(current_page);
		/*alternative unicode text*/
		HPDF_Page_GSave(current_page);
		HPDF_Page_BeginText(current_page);
		HPDF_Page_SetFontAndSize(current_page,using_fonts[current_using_fontname],mm_to_px(font_size_alter));
		HPDF_Page_TextOut(current_page,BL_x,BL_y+grid_height-mm_to_px(font_size_alter),("U+"+to_hex((int)character)).c_str());
		HPDF_Page_EndText(current_page);
		HPDF_Page_GRestore(current_page);
		/*end alternative unicode text*/
	}

	void input_pdf_generator::draw_grid_set(grid_size size,character_info current_char,bool is_Fixed_Based,px BL_x,px BL_y){
		mm info_y_offset=draw_write_grid(size,current_char.width,current_char.g_type,is_Fixed_Based,BL_x,BL_y);
		draw_info_grid(size,current_char.width,BL_x,BL_y+mm_to_px(info_y_offset),current_char.character,current_char.has_alternative);
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
		if(result == nullptr){
			throw std::runtime_error("error: failed to load qrcode to pdf image!");
		}
		delete[] pixels;
		return result;
	}

	void input_pdf_generator::draw_corner_marker(px start_x,px start_y,corner_rotation rotate){
		double rotation = -M_PI_2*(int)rotate;//counterclockwise
		px width = mm_to_px(marker_width_mm);
		px length = mm_to_px(marker_length_mm);
		HPDF_Page_GSave(current_page);
		HPDF_Page_Concat(current_page,cos(rotation),-sin(rotation),sin(rotation),cos(rotation),start_x,start_y);
		HPDF_Page_MoveTo(current_page,0.0,0.0);
		HPDF_Page_LineTo(current_page,length,0.0);
		HPDF_Page_LineTo(current_page,length,-width);
		HPDF_Page_LineTo(current_page,-width,-width);
		HPDF_Page_LineTo(current_page,-width,length);
		HPDF_Page_LineTo(current_page,0.0,length);
		HPDF_Page_LineTo(current_page,0.0,0.0);
		HPDF_Page_Fill(current_page);
		HPDF_Page_GRestore(current_page);
	}

	input_pdf_generator::input_pdf_generator(const std::string fontname){
		pdf = HPDF_New(error_handler,nullptr);
		if(!pdf){
			throw std::runtime_error("error: failed to create pdf document!");
		}
		HPDF_UseUTFEncodings(pdf);
		HPDF_SetCurrentEncoder(pdf,"UTF-8");
		HPDF_SetCompressionMode(pdf,HPDF_COMP_METADATA|HPDF_COMP_IMAGE);
		font_name=fontname;
		std::string qr_places[] = {"TL","TR","BL","BR"};
		for(int i = 1;i<4;i++){
			qr_codes[qr_places[i]] = generate_qr_code(qr_places[i].c_str());
		}
	}

	void input_pdf_generator::add_page(chardef_filemeta filemeta){
		HPDF_Page current_page = HPDF_AddPage(pdf);
		pages.push_back(current_page);
		this->current_page = current_page;
		HPDF_Page_SetSize(current_page, HPDF_PAGE_SIZE_A4,HPDF_PAGE_PORTRAIT);
		chardef_data data;
		data.parse_chardef(filemeta);
		std::string qr_info_tmp="";
		qr_info_tmp = "TL_Handfont_"+filemeta.get_filecode()+"_"+font_name;
		qr_codes["TL"]= generate_qr_code(qr_info_tmp.c_str());
		px constexpr qr_offset=mm_to_px(5.0);
		//px constexpr qr_size=mm_to_px(20.0);
		//px constexpr qr_size_TL=mm_to_px(40.0);
		px qr_size = mm_to_px((mm)HPDF_Image_GetWidth(qr_codes["BR"]));
		px qr_size_TL = mm_to_px((mm)HPDF_Image_GetWidth(qr_codes["TL"]));
		px page_width = HPDF_Page_GetWidth(current_page);
		px page_height= HPDF_Page_GetHeight(current_page);
		
		HPDF_Page_DrawImage(current_page,qr_codes["BL"],qr_offset,qr_offset,qr_size,qr_size);
		HPDF_Page_DrawImage(current_page,qr_codes["BR"],page_width-qr_offset-qr_size,qr_offset,qr_size,qr_size);
		HPDF_Page_DrawImage(current_page,qr_codes["TR"],page_width-qr_offset-qr_size,page_height-qr_offset-qr_size,qr_size,qr_size);
		HPDF_Page_DrawImage(current_page,qr_codes["TL"],qr_offset,page_height-qr_offset-qr_size_TL,qr_size_TL,qr_size_TL);

		for(int i =0;i<data.required_fontnames.size();i++){
			if(using_fonts.find(data.required_fontnames[i])==using_fonts.end()){
				stdfsys::path fontfilepath = stdfsys::path(filemeta.get_rootdir()).parent_path()/"datas"/(data.required_fontnames[i]+".ttf");
				//std::string fontfilename="../../datas/"+data.required_fontnames[i]+".ttf";
				auto font_name = HPDF_LoadTTFontFromFile(pdf,fontfilepath.native().c_str(),HPDF_TRUE);
				//auto font_name = HPDF_LoadTTFontFromFile(pdf,fontfilename.c_str(),HPDF_FALSE);//failed (tofu!)
				using_fonts[data.required_fontnames[i]] = HPDF_GetFont(pdf,font_name,"UTF-8");
			}
		}
		HPDF_Page_SetFontAndSize(current_page,using_fonts[data.required_fontnames[0]],mm_to_px(font_size));
		current_using_fontname=data.required_fontnames[0];

		HPDF_Page_GSave(current_page);
		int current_internal_id=0;
		mm current_BL_x=0.0;
		mm current_BL_y=0.0;
		px height_current_grid=0.0;
		px garea_x_offset = (page_width-mm_to_px(width_grids_area))/2.0;
		px garea_y_offset = (page_height-mm_to_px(height_grids_area))/2.0;

		std::string char_typenames;
		for(const auto& char_typename : data.char_typenames){
			char_typenames += char_typename+" ";
		}
		if(char_typenames.back() == ' '){
			char_typenames.pop_back();
		}
		HPDF_Page_BeginText(current_page);
		mm ascent = font_size*(HPDF_Font_GetAscent(using_fonts[current_using_fontname])/1000.0);
		mm descent = font_size*(HPDF_Font_GetDescent(using_fonts[current_using_fontname])/1000.0);
		mm distance_line = descent + ascent + 2.0 ;
		auto text_width = HPDF_Page_TextWidth(current_page,font_name.c_str());
		HPDF_Page_TextOut(current_page,(page_width-text_width)/2.0,page_height-mm_to_px(5.0)-mm_to_px(distance_line),font_name.c_str());
		text_width = HPDF_Page_TextWidth(current_page,to_string(data.f_type).c_str());
		HPDF_Page_TextOut(current_page,(page_width-text_width)/2.0,page_height-mm_to_px(5.0)-mm_to_px(2*distance_line),to_string(data.f_type).c_str());
		text_width = HPDF_Page_TextWidth(current_page,char_typenames.c_str());
		HPDF_Page_TextOut(current_page,(page_width-text_width)/2.0,page_height-mm_to_px(5.0)-mm_to_px(3*distance_line),char_typenames.c_str());
		HPDF_Page_EndText(current_page);

		draw_corner_marker(0.0+garea_x_offset,garea_y_offset,corner_rotation::BL);
		draw_corner_marker(mm_to_px(width_grids_area)+garea_x_offset,garea_y_offset,corner_rotation::BR);
		draw_corner_marker(mm_to_px(width_grids_area)+garea_x_offset,mm_to_px(height_grids_area)+garea_y_offset,corner_rotation::TR);
		draw_corner_marker(0.0+garea_x_offset,mm_to_px(height_grids_area)+garea_y_offset,corner_rotation::TL);
		//HPDF_Page_Concat(current_page,1,0,0,1,10.0,10.0);
		for(character_info current_char:data.char_infos){
			if(current_char.width == char_width::FULL 
					&& ((data.size == grid_size::SMALL && current_internal_id%num_internal_cols::SMALL == num_internal_cols::SMALL-1)
						||(data.size == grid_size::LARGE && current_internal_id%num_internal_cols::LARGE == num_internal_cols::LARGE-1))
				)
			{
				current_internal_id++;
			}
			switch(data.size){
				case grid_size::SMALL:
					if(data.is_Fixed_Base){
						height_current_grid=height_grid::SMALL*(3.0/2.0);
					}else{
						height_current_grid=height_grid::SMALL;
					}
					current_BL_y=height_grids_area-((floor((double)current_internal_id/num_internal_cols::SMALL)+1)*(height_current_grid+height_grid::SMALL));
					current_BL_x=(current_internal_id%num_internal_cols::SMALL)*(height_grid::SMALL/2.0);
					break;
				case grid_size::LARGE:
					if(data.is_Fixed_Base){
						height_current_grid=height_grid::LARGE*(3.0/2.0);
					}else{
						height_current_grid=height_grid::LARGE;
					}
					current_BL_y=height_grids_area-((floor((double)current_internal_id/num_internal_cols::LARGE)+1)*(height_current_grid+height_grid::SMALL));
					current_BL_x=(current_internal_id%num_internal_cols::LARGE)*(height_grid::LARGE/2.0);
					break;
			}
			draw_grid_set(data.size,current_char,data.is_Fixed_Base,garea_x_offset+mm_to_px(current_BL_x),garea_y_offset+mm_to_px(current_BL_y));
			if(current_char.width == char_width::HALF){
				current_internal_id+=1;
			}else{
				current_internal_id+=2;
			}
		}
		/*
		draw_grid_set(current_page,data.size,data.char_infos[0],data.is_Fixed_Base,0.0,0.0);
		draw_grid_set(current_page,data.size,data.char_infos[1],data.is_Fixed_Base,0.0+(mm_to_px(height_grid::SMALL)),0.0);
		*/
		HPDF_Page_GRestore(current_page);
	}

	void input_pdf_generator::saveto_file(const std::string filename){
		HPDF_SaveToFile(pdf,filename.c_str());
	}

	input_pdf_generator::~input_pdf_generator(){
		HPDF_Free(pdf);
	}
}
