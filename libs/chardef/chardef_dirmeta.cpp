#include "chardef_dirmeta.hpp"

#include <toml.hpp>

namespace handfont {
ChardefDirmeta::ChardefDirmeta(std::filesystem::path dir_path) {
    auto metadata = toml::parse(dir_path / "metadata.toml");
    id_ = toml::find<std::string>(metadata, "id");
    version_ = toml::find<int>(metadata, "version");
    path_ = dir_path;
}
ChardefDirmeta ChardefDirmeta::from_id(std::vector<std::filesystem::path> rootdirs, std::string id, int version) {
    ChardefDirmeta result;
    for (const auto& chardefset_rootdir : rootdirs) {
        for (const auto& chardefset_dir : std::filesystem::directory_iterator(chardefset_rootdir)) {
            handfont::ChardefDirmeta dirmeta(chardefset_dir);
            if (dirmeta.id() == id && dirmeta.version() == version) {
                result = dirmeta;
                break;
            }
        }
    }
    return result;
}
std::string ChardefDirmeta::id() { return id_; }
int ChardefDirmeta::version() { return version_; }
std::filesystem::path ChardefDirmeta::path() { return path_; }
bool ChardefDirmeta::is_valid() { return not path_.empty(); }
}  // namespace handfont
