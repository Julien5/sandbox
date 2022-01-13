#include "adcfile.h"
#include "common/time.h"
#ifdef PC
#include <cassert>
#include "common/debug.h"
#include <fstream>

#include "parameters.h"

namespace {
    std::string read_file(const std::string &fileName) {
        std::ifstream ifs(fileName.c_str(), std::ios::in | std::ios::ate);
        std::ifstream::pos_type s = ifs.tellg();
        ifs.seekg(0, std::ios::beg);
        std::vector<char> bytes(s);
        ifs.read(bytes.data(), s);
        return std::string(bytes.data(), s);
    }

    std::vector<std::string> split(const std::string &s, const std::string &sep) {
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

    std::vector<std::string> lines(const std::string &s) {
        auto ret = split(s, "\n");
        return ret;
    }
}

adcfile *adcfile::instance() {
    static adcfile *inst = nullptr;
    if (!inst)
        inst = new adcfile();
    return inst;
}

adcfile::adcfile() {
    if (parameters::get().empty())
        throw std::runtime_error("missing filename parameter");
    auto filename = parameters::get().at(0);
    m_lines = lines(read_file(filename));
}

void adcfile::setT(const int &T) {
    m_T = T;
}

u16 adcfile::read() {
    assert(m_T > 0);
    // last line is empty
    if (m_line_index >= (m_lines.size() - 1)) {
        DBG("done.ok.");
        exit(0);
    }
    auto line = m_lines[m_line_index++];
    common::time::simulate(common::time::us(100));
    auto ret = std::stoi(line);
    DBG("common::analog::read:%4d [line:%6d]\r\n", int(ret), int(m_line_index - 1));
    return ret;
}
#endif
