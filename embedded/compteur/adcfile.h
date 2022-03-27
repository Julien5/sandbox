#pragma once
#ifdef PC
#include <vector>
#include <string>
#include "common/analog.h"

class adcfile : public common::analog_read_callback {
    std::vector<std::string> m_lines;
    size_t m_line_index = 0;
    size_t m_element_index = 0;

  public:
    adcfile();
    u16 read();
};
#else
class adcfile {};
#endif
