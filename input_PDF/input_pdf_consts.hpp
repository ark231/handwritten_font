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
}
#endif
