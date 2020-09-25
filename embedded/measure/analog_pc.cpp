#include "analog.h"

u16 analog::read() {
    return u16(std::rand());
}
