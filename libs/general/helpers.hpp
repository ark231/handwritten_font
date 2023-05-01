#ifndef HANDFONT_HELPERS
#define HANDFONT_HELPERS
#include <filesystem>
#include <string>

#include "chardef/chardef_consts.hpp"
namespace stdfsys = std::filesystem;
namespace handfont {
std::string to_hex(int);
std::string to_hex(int, int, const char);
stdfsys::path get_self_dir();
}  // namespace handfont
#endif
