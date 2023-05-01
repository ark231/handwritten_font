#ifndef HANDFONT_CHARDEF_FILEMETA
#define HANDFONT_CHARDEF_FILEMETA
#include <filesystem>
#include <string>
#include <vector>

#include "chardef_consts.hpp"
namespace stdfsys = std::filesystem;
namespace handfont {
std::vector<int> search_available_files(std::string, grid_size, font_type);
std::vector<int> search_available_files(std::string, std::string, std::string);
class chardef_filemeta {
    grid_size size;
    font_type type;
    UInt file_id;
    std::string rootdir;
    // filecode code;
    void set_metadatas(grid_size, font_type, UInt, std::string);

   public:
    chardef_filemeta(filecode, std::string);
    chardef_filemeta(grid_size, font_type, UInt, std::string);
    chardef_filemeta();
    void set_metadatas(filecode, std::string);
    std::string get_path();
    std::string get_rootdir();
    filecode get_filecode();
    stdfsys::path dir_order(std::string);
};
}  // namespace handfont
#endif
