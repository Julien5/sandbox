#include "adcfile.h"
#include "common/time.h"
#ifdef PC
#include <cassert>
#include "common/debug.h"
#include <fstream>
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
    m_lines = lines(read_file("output"));
}

void adcfile::setT(const int &T) {
    m_T = T;
}

u16 adcfile::read() {
    assert(m_T > 0);
    // last line is empty
    if (m_line_index >= (m_lines.size() - 1)) {
        m_line_index = 0;
        m_element_index = 0;
    }
    auto line = m_lines[m_line_index];
    //DBG("line:%s [%d]\r\n", line.c_str(), int(m_line_index));
    auto elements = split(line, " ");
    assert(m_element_index < elements.size());
    assert(m_element_index < m_T);
    auto element = elements[m_element_index++];
    if (m_element_index == m_T) {
        m_line_index++;
        m_element_index = 0;
    }
    common::time::simulate(common::time::us(100));
    assert(!element.empty());
    return std::stoi(element);
}
#endif
