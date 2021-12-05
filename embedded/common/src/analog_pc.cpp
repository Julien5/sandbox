#include "common/analog.h"
#include <cassert>
#include "common/debug.h"
#include <memory>
namespace {
    static std::unique_ptr<common::analog_read_callback> callback;
}

void common::analog_read_callback::install(analog_read_callback *addr) {
    callback.reset(addr);
}

u16 common::analog_read_callback::read() {
    return 0;
}

u16 common::analog::read() {
    if (callback)
        return callback->read();
    return 0;
}
