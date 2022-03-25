#include "adcfile.h"
#include "common/time.h"
#ifdef PC
#include <cassert>
#include "common/debug.h"
#include "read_file.h"

#include "parameters.h"

adcfile *adcfile::instance() {
    static adcfile *inst = nullptr;
    if (!inst)
        inst = new adcfile();
    return inst;
}

adcfile::adcfile() {
    using namespace read_file;
    if (parameters::get().empty()) {
        return;
    }
    auto filename = parameters::get().at(0);
    m_lines = lines(content(filename));
    assert(!m_lines.empty());
}

u16 adcfile::read() {
    if (m_lines.empty())
        throw std::runtime_error("missing filename parameter");
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
