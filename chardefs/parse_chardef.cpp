#include<fstream>
#include<iostream>
#include<string>
#include<stdexcept>
#include"parse_chardef.hpp"
namespace handfont{
	void chardef_data::parse_chardef(std::filesystem::path chardef_filepath){
		std::vector<character_info> *result = new std::vector<character_info>;
		auto chardef_file = std::ifstream(chardef_filepath.native());
		if(chardef_file.fail()){
			throw std::runtime_error("error: could not open chardef file");
		}
		std::string line;
		while(std::getline(chardef_file,line)){
			if(line[0]=='#'||line.empty()){
				continue;
			}
			std::string uncommented_line;
			if(line.find("#",0)==std::string::npos){
				uncommented_line=line;
			}else{
				uncommented_line=line.substr(0,line.find("#",0));
			}
			if(uncommented_line[0]=='['){//character type list
				int endof_list;
				if((endof_list=uncommented_line.find("]",0))==std::string::npos){
					throw std::runtime_error("error: character type list is not closed!");
				}
				std::string list_all_raw=uncommented_line.substr(1,endof_list-1);
				int start_val=0;
				int place_comma;
				while((place_comma=list_all_raw.find(",",start_val))!=std::string::npos){
					char_typenames.push_back(list_all_raw.substr(start_val,place_comma-start_val));
					start_val+=place_comma-start_val+1;
				}
				char_typenames.push_back(list_all_raw.substr(start_val));//there are no comma after the last value 
			}else if(uncommented_line[0]=='H'||uncommented_line[0]=='F'){//chardef line!!
				character_info char_tmp;
				if(uncommented_line[0]=='H'){
					char_tmp.width = char_width::HALF;
				}else if(uncommented_line[0]=='F'){
					char_tmp.width = char_width::FULL;
				}
				char_tmp.character=(Unicode)std::stoi(uncommented_line.substr(4),nullptr,16);
				char_infos.push_back(char_tmp);
			}else if(uncommented_line[0]=='%'){
				if(uncommented_line.find("small",0)!=std::string::npos){
					size=grid_size::SMALL;
				}else if(uncommented_line.find("large",0)!=std::string::npos){
					size=grid_size::LARGE;
				}else{
					throw std::runtime_error("error: invalid grid size \""+uncommented_line+"\"");
				}
			}
		}
	}
}
