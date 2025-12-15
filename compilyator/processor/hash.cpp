#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#include "hash.h"

uint64_t StackHash(void* buf, size_t cnt_bytes) {
    assert(buf);

    char* temp_buf = (char*)buf;

    uint64_t check_sum = 1;

    for (size_t i = 0; i < cnt_bytes; i++) {
        check_sum += (check_sum * (uint64_t)(*temp_buf) + DEVIL_NUMBER);
        temp_buf++;
    }

    return check_sum;
}