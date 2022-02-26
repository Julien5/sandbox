#include "read_file.h"
#include <fstream>

std::string read_file::content(const std::string &fileName) {
    std::ifstream ifs(fileName.c_str(), std::ios::in | std::ios::ate);
    std::ifstream::pos_type s = ifs.tellg();
    ifs.seekg(0, std::ios::beg);
    std::vector<char> bytes(s);
    ifs.read(bytes.data(), s);
    return std::string(bytes.data(), s);
}
std::vector<std::string> read_file::split(const std::string &s, const std::string &sep) {
    size_t beg = 0;
    size_t end = 0;
    std::vector<std::string> ret;
    while (true) {
        end = s.find(sep, beg);
        auto part = s.substr(beg, end == std::string::npos ? end : end - beg);
        ret.push_back(part);
        if (end == std::string::npos)
            break;
        beg = end + 1;
        end = 0;
    }
    return ret;
}

std::vector<std::string> read_file::lines(const std::string &s) {
    auto ret = split(s, "\n");
    return ret;
}
