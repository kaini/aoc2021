#include "parse.h"
#include <assert.h>

int parse_binary(const char* str) {
    assert(str);
    int result = 0;
    for (const char* c = str; *c; ++c) {
        result <<= 1;
        if (*c == '1') result += 1;
    }
    return result;
}
