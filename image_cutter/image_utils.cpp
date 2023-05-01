#include "image_utils.hpp"

#include <cmath>
#include <stdexcept>

namespace handfont {
dpmm dpi_to_dpmm(dpi input_dpi) { return (input_dpi * inch_per_mm); }
bool is_close(const double& one, const double& theother, const double aboutness, const close_type type) {
    double diff = abs((theother - one));
    bool result = false;
    switch (type) {
        case close_type::REL:
            result = (diff < one * aboutness);
            break;
        case close_type::ABS:
            result = (diff < aboutness);
            break;
    }
    return result;
}
}  // namespace handfont
