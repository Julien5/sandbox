#include "adcfile.h"
#include "common/time.h"
#ifdef PC
#include <cassert>
#include "common/debug.h"
#include "read_file.h"

#include "parameters.h"

adcfile::adcfile() {
    using namespace read_file;
    if (parameters::get().empty()) {
        return;
    }
    auto filename = parameters::get().at(0);
    std::vector<std::string> L = lines(content(filename));
    for (const auto &l : L) {
        // DBG("%s\n", l.c_str());
        // [1200766]-> value:511 xalpha:518 delta:007 threshold:072
        // [1200766] value:511 xalpha:518 delta:007 threshold:072
        bool good = !l.empty() && l[0] == '[' && l.find(" value:") != std::string::npos;
        if (!good)
            continue;
        auto parts = split(l, " ");
        for (auto part : parts) {
            if (part[0] == '[')
                continue;
            auto pair = split(part, ":");
            if (pair.size() != 2)
                continue;
            auto name = pair[0];
            auto value = pair[1];
            if (name == "value")
                m_values.push_back(std::stoi(value));
        }
    }
    assert(!m_values.empty());
}

u16 adcfile::read() {
    assert(!m_values.empty());
    // last line is empty
    if (m_index >= (m_values.size() - 1)) {
        DBG("done.ok.");
        //exit(0);
        m_index = 0;
    }
    auto ret = m_values[m_index++];
    common::time::simulate(common::time::us(100));
    // DBG("common::analog::read:%4d [line:%6d]\r\n", int(ret), int(m_index - 1));
    return ret;
}
#endif
