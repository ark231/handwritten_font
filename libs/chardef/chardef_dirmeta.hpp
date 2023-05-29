#ifndef HANDFONT_CHARDEF_DIRMETA
#define HANDFONT_CHARDEF_DIRMETA
#include <filesystem>
#include <string>
#include <vector>
namespace handfont {
class ChardefDirmeta {
    std::string id_;
    int version_;
    std::filesystem::path path_;

   public:
    explicit ChardefDirmeta(std::filesystem::path);
    static ChardefDirmeta from_id(std::vector<std::filesystem::path> rootdirs, std::string id, int version);
    ChardefDirmeta() {}
    std::string id();
    int version();
    std::filesystem::path path();
    bool is_valid();
};
}  // namespace handfont
#endif
