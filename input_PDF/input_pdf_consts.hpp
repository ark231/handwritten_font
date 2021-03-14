#ifndef HANDFONT_INPUT_PDF_CONSTS
#define HANDFONT_INPUT_PDF_CONSTS
namespace handfont{
	using px=double;
	using mm=double;
	double constexpr dpi = 72.0;
	double constexpr mm_per_inch = 25.4;
	namespace height_grid{
		mm constexpr SMALL=7.0;
		mm constexpr LARGE=14.0;
	}
	namespace num_internal_cols{
		int constexpr SMALL=48;
		int constexpr LARGE=24;
	}
	mm constexpr height_grids_area = height_grid::SMALL*30;
	mm constexpr width_grids_area = height_grid::SMALL*24;
	mm constexpr font_size = 5.0;
	mm constexpr font_size_alter = 1.0;
}
#endif
