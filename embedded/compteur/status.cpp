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

u16 status::Data::get(const usize &index) {
    return D[index];
};

void status::Data::set(const usize &indx, const u16 &value) {
    return;
    if (indx == status::index::M) {
        //DBG("M:%d\n", value);
        assert(value != 0);
    }
    D[indx] = value;
};

const u8 *status::Data::data(usize *L) const {
    *L = sizeof(D);
    return (u8 *)(&D);
}

void status::Data::dump() {
    assert(D[status::index::M] != 0);
    DBG("status: %d %d %d %d %d\r\n", D[status::index::m], D[status::index::TL], D[status::index::TH], int(D[status::index::M]), D[status::index::line]);
}
