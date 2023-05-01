#ifndef HANDFONT_INPUT_PDF_CONSTS
#define HANDFONT_INPUT_PDF_CONSTS
namespace handfont {
using px = double;
using mm = double;
double constexpr dpi = 72.0;
double constexpr mm_per_inch = 25.4;
namespace height_grid {
mm constexpr SMALL = 7.0;
mm constexpr LARGE = 14.0;
}  // namespace height_grid
namespace num_internal_cols {
int constexpr SMALL = 48;
int constexpr LARGE = 24;
}  // namespace num_internal_cols
mm constexpr height_grids_area = height_grid::SMALL * 30;
mm constexpr width_grids_area = height_grid::SMALL * 24;
mm constexpr font_size = 5.0;
mm constexpr font_size_alter = 1.0;
mm constexpr marker_width_mm = 5.0;
mm constexpr marker_length_mm = 14.0;
namespace grid_color {
double constexpr R = 0.9;
double constexpr G = 1.0;
double constexpr B = 1.0;
}  // namespace grid_color
namespace fill_color {
double constexpr R = 0.9;
double constexpr G = 1.0;
double constexpr B = 1.0;
}  // namespace fill_color
enum class corner_rotation { BL = 0, BR, TR, TL };
}  // namespace handfont
#endif
