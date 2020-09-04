#include "ui.h"

#include <stdio.h>
#include <string.h>
#include <math.h>
#include "debug.h"
#include "utils.h"
#include "platform.h"

char FMT[][5] = {
    "%0li",
    "%1li",
    "%2li",
    "%3li",
    "%4li",
    "%5li",
    "%6li",
    "%7li",
    "%8li",
};

void ui::format(i32 n, char *buffer, size_t L) {
    L = xMin(L, size_t(8));

    // maxlength = L
    int E = 0;
    int M = int(pow(10, L - 1));
    if (n < M) {
        snprintf(buffer, L, FMT[L - 1], n);
        return;
    }

    // try 1000 => 1k
    if (n < 100 * M) {
        n = n / 1000;
        snprintf(buffer, L, FMT[L - 2], n);
        buffer[L - 2] = 'k';
        buffer[L - 1] = 0;
        return;
    }

    while (n >= (M / 100)) {
        n = n / 10;
        E++;
    }
    snprintf(buffer, L, "%lie%d", n, E);
}

void ui::format_seconds(i32 secs, char *buffer, size_t L) {
    if (L < 2) {
        snprintf(buffer, L, "E%li", secs);
        return;
    }
    L = xMin(L, size_t(8));

    int M = int(pow(10, L - 2));
    if (secs < M) {
        snprintf(buffer, L - 1, FMT[L - 2], secs);
        buffer[L - 2] = 's';
        buffer[L - 1] = 0;
        return;
    }

    M = M * 10;
    if (secs < M) {
        snprintf(buffer, L, FMT[L - 1], secs);
        return;
    }

    M = M / 10;
    int minutes = secs / 60;
    if (minutes < M) {
        snprintf(buffer, L - 1, FMT[L - 2], minutes);
        buffer[L - 2] = 'm';
        buffer[L - 1] = 0;
        return;
    }

    snprintf(buffer, L, "E%li", secs);
}

int ui::test() {
    constexpr int L = 4;
    char buffer[128] = {0};
    format(10, buffer, L);
    DBG(buffer);
    assert(strcmp(buffer, " 10") == 0);

    format(123, buffer, L);
    DBG(buffer);
    assert(strcmp(buffer, "123") == 0);

    format(123, buffer, L - 1);
    DBG(buffer);
    assert(strcmp(buffer, "0k") == 0);

    format(9876, buffer, L);
    DBG(buffer);
    assert(strcmp(buffer, " 9k") == 0);

    format(9876, buffer, L + 1);
    DBG(buffer);
    assert(strcmp(buffer, "9876") == 0);

    format(19876, buffer, L + 1);
    DBG(buffer);
    assert(strcmp(buffer, " 19k") == 0);

    format(19876, buffer, 8);
    DBG(buffer);
    assert(strcmp(buffer, "  19876") == 0);

    format_seconds(44, buffer, 3);
    DBG(buffer);
    assert(strcmp(buffer, "44") == 0);

    format_seconds(44, buffer, 4);
    DBG(buffer);
    assert(strcmp(buffer, "44s") == 0);

    format_seconds(74, buffer, 3);
    DBG(buffer);
    assert(strcmp(buffer, "74") == 0);

    format_seconds(174, buffer, 3);
    DBG(buffer);
    assert(strcmp(buffer, "2m") == 0);

    format_seconds(10 * 60, buffer, 3);
    DBG(buffer);
    assert(strcmp(buffer, "E6") == 0);

    format_seconds(10 * 60, buffer, 4);
    DBG(buffer);
    assert(strcmp(buffer, "600") == 0);

    format_seconds(10, buffer, 4);
    DBG(buffer);
    assert(strcmp(buffer, "10s") == 0);

    format_seconds(60 * 60, buffer, 4);
    DBG(buffer);
    assert(strcmp(buffer, "60m") == 0);

    return 0;
}
