#ifndef STACK_H_
#define STACK_H_

#include <stdio.h>
#include <stdint.h>

#include "../back_common.h"
#include "debug.h"
#include "hash.h"
#include "variable_information.h"

#ifdef DEBUG
#define STACK_CREATE(stack, start_capacity) StackCtorDebug(&stack, start_capacity, {#stack, __LINE__, __func__, __FILE__})
#else // NOT DEBUG
#define STACK_CREATE(stack, start_capacity) StackCtor(&stack, start_capacity)
#endif // DEBUG

#define STACK_CHECK_ERRORS(error, ...)                             \
        if (error != STACK_SUCCESS) {                              \
            ON_DEBUG(StackDump(stack, error, DUMP_VAR_INFO));      \
            __VA_ARGS__;                                           \
            return error;                                          \
        }


const char * const TYPE_NAME = "int";

const size_t DEFAULT_START_CAPACITY = 32; // it must be > 0
const size_t MAX_CAPACITY           = 1e9;

const int DEFAULT_POISON = 0XDED;

const int REALLOC_COEFF = 2;


struct stack_t {
    ON_CANARY(int first_elem);
    type_t* data = NULL;
    size_t size = 0;
    size_t capacity = 0;
    ON_DEBUG(debug_info_t debug_info);
    ON_HASH(uint64_t hash_struct = 0);
    ON_HASH(uint64_t hash_data   = 0);
    ON_CANARY(int last_elem);
};


enum stack_status {
    STACK_SUCCESS             = 0,
    NULL_POINTER_ON_STRUCT    = 1 << 0,
    NULL_POINTER_ON_DATA      = 1 << 1,
    TOO_BIG_CAPACITY          = 1 << 2,
    TOO_BIG_SIZE              = 1 << 3,
    SIZE_BIGGER_THAN_CAPACITY = 1 << 4,
    NOT_ENOUGH_MEMORY         = 1 << 5, 
    POP_EMPTY_STACK           = 1 << 6,
    STACK_DATA_IS_POISON      = 1 << 7,
    STACK_OVERFLOW            = 1 << 8,
    CORRUPTED_DATA_CANARY     = 1 << 9,
    CORRUPTED_STRUCT_CANARY   = 1 << 10,
    CORRUPTED_STRUCT_HASH     = 1 << 11,
    CORRUPTED_DATA_HASH       = 1 << 12
};


size_t RealSizeStack(size_t capacity, size_t cout_canaries);

size_t OffsetDueCanaries(size_t count_canaries);

size_t OffsetToLastElement(size_t size, size_t count_canaries);

size_t OffsetToNewElement(size_t size, size_t count_canaries);

size_t RealIndex(size_t index, size_t count_canaries);

ON_HASH(void SetHash(stack_t* stack));

type_error_t StackCtor(stack_t* stack, size_t start_capacity);

ON_DEBUG(type_error_t StackCtorDebug(stack_t* stack, size_t start_capacity, debug_info_t my_var_info));

type_error_t StackReSize(stack_t* stack, size_t old_capacity);

type_error_t StackPush(stack_t* stack, type_t new_value);

type_error_t StackPeek(stack_t* stack, type_t* peek_element);

type_error_t StackPop(stack_t* stack, type_t* delete_value);

type_error_t StackDtor(stack_t* stack);


#endif //STACK_H_