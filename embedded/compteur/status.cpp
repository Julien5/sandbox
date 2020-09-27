#include "status.h"
#include <string.h>
#include "common/debug.h"

status::Data status::instance;

status::Data::Data() {
    memset(&D, 0, sizeof(D) / sizeof(D[0]));
}

status::Data status::Data::get() {
    static status::Data s_status;
    return s_status;
}

u8 status::Data::get(const usize &index) {
    return D[index];
};

void status::Data::set(const usize &indx, const u8 &value) {
    D[indx] = value;
};

const u8 *status::Data::data(usize *L) const {
    *L = sizeof(D);
    return (u8 *)(&D);
}
