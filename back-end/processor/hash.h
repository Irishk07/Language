#ifndef HASH_H_
#define HASH_H_

#include <stdint.h>
#include <stdio.h>

#ifdef HASH
#define ON_HASH(...) __VA_ARGS__
#else //NO HASH
#define ON_HASH(...)
#endif // HASH


const size_t DEVIL_NUMBER = 13;


uint64_t StackHash(void* buf, size_t cnt_bytes);


#endif // HASH_H_