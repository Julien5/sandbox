#include "sleep_authorization.h"

namespace {
    static bool s_authorized = true;
}

void sleep_authorization::reset() {
    s_authorized = true;
}

void sleep_authorization::forbid() {
    s_authorized = false;
}

bool sleep_authorization::authorized() {
    return s_authorized;
}
