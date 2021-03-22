#include"image_utils.hpp"

namespace handfont{
	dpmm dpi_to_dpmm(dpi input_dpi){
		return (input_dpi*inch_per_mm);
	}
}
