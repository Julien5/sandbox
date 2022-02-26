#pragma once

#include <string>
#include <vector>

namespace read_file {
    std::string content(const std::string &fileName);
    std::vector<std::string> split(const std::string &s, const std::string &sep);
    std::vector<std::string> lines(const std::string &s);
}
