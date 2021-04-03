#include"image_utils.hpp"
#include<cmath>
#include<stdexcept>

namespace handfont{
	dpmm dpi_to_dpmm(dpi input_dpi){
		return (input_dpi*inch_per_mm);
	}
}
