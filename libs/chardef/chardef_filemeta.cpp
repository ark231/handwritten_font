#include<stdexcept>
#include<string>
#include<filesystem>
#include<vector>
#include<regex>
//#include<sstream>
//#include<iomanip>

#include"chardef_filemeta.hpp"
#include"chardef_convert_consts.hpp"
#include"general/helpers.hpp"
namespace handfont{
	stdfsys::path chardef_dirorder(std::string rootdir,grid_size size,font_type type){
		stdfsys::path dirpath_size,dirpath_type,filename_path;
		dirpath_size=stdfsys::path(to_string(size));
		dirpath_type=stdfsys::path(to_string(type));
		return stdfsys::path(rootdir)/dirpath_size/dirpath_type;
	}

	std::vector<int> search_available_files(std::string rootdir,grid_size size,font_type type){
		std::vector<int> result;
		std::smatch smatch;
		for(const auto& filename : stdfsys::directory_iterator(chardef_dirorder(rootdir,size,type))){
			if(std::regex_search(filename.path().native(),smatch,std::regex("/[0-9a-f]+\\.cdef"))){
				std::smatch raw_file_id;
				auto found_filename = smatch.str();
				std::regex_search(found_filename,raw_file_id,std::regex("[0-9a-f]+"));
				auto file_id = std::stoi(raw_file_id.str(),nullptr,16);
				result.push_back(file_id);
			}
		}
		return result;
	}
	std::vector<int> search_available_files(std::string rootdir,std::string size,std::string type){
		return search_available_files(rootdir,to_grid_size(size),to_font_type(type));
	}

	void chardef_filemeta::set_metadatas(grid_size size,font_type type,UInt id,std::string rootdir){
		this->size = size;
		this->type = type;
		this->file_id = id;
		this->rootdir = rootdir;
	}

	void chardef_filemeta::set_metadatas(filecode code,std::string rootdir){
		grid_size size_tmp;
		font_type type_tmp;
		UInt id_tmp;
		switch(code[0]){
			case 'S':
				size_tmp = grid_size::SMALL;
				break;
			case 'L':
				size_tmp = grid_size::LARGE;
				break;
			default:
				throw std::runtime_error("error: unknown grid_size: '"+code+"'");
				break;
		}
		switch(code[1]){
			case 'M':
				type_tmp = font_type::MONO;
				break;
			case 'P':
				type_tmp = font_type::PROPORTIONAL;
				break;
			default:
				throw std::runtime_error("error: unknown font_type: '"+code+"'");
				break;
		}
		id_tmp = std::stoi(code.substr(3),nullptr,16);
		set_metadatas(size_tmp,type_tmp,id_tmp,rootdir);
	}

	chardef_filemeta::chardef_filemeta(){}

	chardef_filemeta::chardef_filemeta(filecode code,std::string rootdir){
		set_metadatas(code,rootdir);
	}

	chardef_filemeta::chardef_filemeta(grid_size size,font_type type,UInt id,std::string rootdir){
		set_metadatas(size,type,id,rootdir);
	}

	std::string chardef_filemeta::get_path(){
		/*
		std::stringstream filename;
		filename<<std::setw(4)<<std::setfill('0')<<std::hex<<file_id;
		*/
		auto filename_path = stdfsys::path(to_hex(file_id,4,'0')+".cdef");
		stdfsys::path result = chardef_dirorder(this->rootdir,this->size,this->type)/filename_path;
		return result.native();
	}

	std::string chardef_filemeta::get_rootdir(){
		return rootdir;
	}

	filecode chardef_filemeta::get_filecode(){
		filecode result="";
		switch(this->size){
			case grid_size::SMALL:
				result+="S";
				break;
			case grid_size::LARGE:
				result+="L";
				break;
		}
		switch(this->type){
			case font_type::MONO:
				result+="M";
				break;
			case font_type::PROPORTIONAL:
				result+="P";
				break;
		}
		std::stringstream code_file_id;
		code_file_id<<std::setw(4)<<std::setfill('0')<<std::hex<<file_id;
		result+="_"+code_file_id.str();
		return result;
	}
	stdfsys::path chardef_filemeta::dir_order(std::string rootdir){
		return chardef_dirorder(rootdir,this->size,this->type);
	}
}
