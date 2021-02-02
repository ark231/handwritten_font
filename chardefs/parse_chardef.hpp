#ifndef HANDFONT_PARSE_CHARDEF
#define HANDFONT_PARSE_CHARDEF
#include<vector>
#include<filesystem>
namespace handfont{
	enum class grid_size{
		SMALL,
		LARGE
	};
	enum class char_width{
		HALF,
		FULL
	};
	enum class index_grid_id{
		LEFT=0,
		RIGHT,
		NUM_ALL
	};
	using UInt=unsigned int;
	using Unicode=uint32_t;
	struct character_info{
		Unicode character;
		char_width width;
		UInt Internal_Grid_IDs[(int)index_grid_id::NUM_ALL];
	};
	struct chardef_data{
		grid_size size;
		std::vector<std::string> char_typenames;
		std::vector<character_info> char_infos;
		void parse_chardef(std::filesystem::path);
	};
}
#endif
