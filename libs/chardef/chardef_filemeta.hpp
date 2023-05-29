#ifndef HANDFONT_CHARDEF_FILEMETA
#define HANDFONT_CHARDEF_FILEMETA
#include <filesystem>
#include <string>
#include <vector>

#include "chardef_consts.hpp"
#include "chardef_dirmeta.hpp"
namespace stdfsys = std::filesystem;
namespace handfont {
std::vector<int> search_available_files(std::string, grid_size, font_type);
std::vector<int> search_available_files(std::string, std::string, std::string);
class chardef_filemeta {
    grid_size size_;
    font_type type_;
    UInt file_id_;
    ChardefDirmeta rootdir_;
    // filecode code;
    void set_metadatas_(grid_size, font_type, UInt, ChardefDirmeta);

   public:
    chardef_filemeta(FileCode, ChardefDirmeta);
    chardef_filemeta(grid_size, font_type, UInt, ChardefDirmeta);
    chardef_filemeta();
    void set_metadatas(FileCode, ChardefDirmeta);
    std::string path();
    ChardefDirmeta rootdir();
    FileCode get_filecode();
    stdfsys::path dir_order(std::string);
};
}  // namespace handfont
#endif
