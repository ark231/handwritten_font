#ifndef HANDFONT_PARSE_CHARDEF
#define HANDFONT_PARSE_CHARDEF
#include<vector>
#include"chardef_consts.hpp"
#include"chardef_filemeta.hpp"
namespace handfont{
	struct other_char{
		chardef_filemeta filemeta;
		Unicode character;
		void set_all(Unicode,filecode,std::string);
	};
	struct character_info{
		Unicode character;
		char_width width;
		guide_type g_type;
		UInt Internal_Grid_IDs[(int)index_grid_id::NUM_ALL];
		bool has_alternative;
		other_char alternative;
	};
	struct chardef_data{
		grid_size size;
		font_type f_type;
		bool is_Fixed_Base;
		std::vector<std::string> char_typenames;
		std::vector<std::string> required_fontnames;
		std::vector<character_info> char_infos;
		void parse_chardef(chardef_filemeta);
	};
}
#endif
