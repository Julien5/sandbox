#include "Arduino.h"

namespace analog {
    u16 read() {
        return analogRead(0);
    }
};
