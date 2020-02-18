#include "options.hh"

#include <fstream>
#include <iterator>

namespace rules
{

std::string read_map_from_path(const std::string& path)
{
    if (path.empty())
        return {};
    std::ifstream ifs{path};
    if (!ifs.is_open())
        FATAL("Cannot open map file: %s", path.c_str());
    std::string map_content((std::istreambuf_iterator<char>(ifs)),
                            std::istreambuf_iterator<char>());
    return map_content;
}

} // namespace rules
