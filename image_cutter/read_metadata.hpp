#ifndef HANDFONT_IMAGE_READ_METADATA
#define HANDFONT_IMAGE_READ_METADATA
#include <filesystem>
#include <string>

#include "image_utils.hpp"
namespace stdfsys = std::filesystem;
namespace handfont {
class image_metadata {
    std::string unit;
    double raw_resolution_x;
    double raw_resolution_y;

   public:
    image_metadata(stdfsys::path filepath);
    dpmm get_dpmm();
};
}  // namespace handfont

#endif
