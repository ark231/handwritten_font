#ifndef HANDFONT_CHARDEF_CONSTS
#define HANDFONT_CHARDEF_CONSTS
#include<string>
namespace handfont{
	enum class grid_size{
		SMALL,
		LARGE
	};
	enum class guide_type{
		NONE,
		FOUR,
		CROSS
	};
	enum class char_width{
		HALF,
		FULL
	};
	enum class font_type{
		MONO,
		PROPORTIONAL
	};
	enum class index_grid_id{
		LEFT=0,
		RIGHT,
		NUM_ALL
	};
	using UInt=unsigned int;
	using Unicode=uint32_t;
	using filecode=std::string;
}
#endif
