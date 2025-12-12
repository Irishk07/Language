#include <assert.h>
#include <stdlib.h>

#include "stack.h"

#include "canary.h"
#include "check_errors.h"
#include "hash.h"
#include "stack.h"
#include "string_functions.h"
#include "variable_information.h"


size_t RealSizeStack(size_t capacity, size_t count_canaries) {
    return (capacity + count_canaries);
} 

size_t OffsetDueCanaries(size_t count_canaries) {
    return (count_canaries == 0) ? 0 : 1;
}

size_t OffsetToLastElement(size_t size, size_t count_canaries) { // FIXME
    return size + OffsetDueCanaries(count_canaries) - 1;
}

size_t OffsetToNewElement(size_t size, size_t count_canaries) {
    return size + OffsetDueCanaries(count_canaries);
}

size_t RealIndex(size_t index, size_t count_canaries) {
    return index + OffsetDueCanaries(count_canaries);
}

ON_HASH (
void SetHash(stack_t* stack) {
    assert(stack);
    assert(stack->data);

    stack->hash_struct = 0;

    stack->hash_data   = StackHash(stack->data, RealSizeStack(stack->capacity, CNT_CANARIES) * sizeof(type_t));
    stack->hash_struct = StackHash(stack, sizeof(*stack));
}
)

type_error_t StackCtor(stack_t* stack, size_t start_capacity) { // TODO: test stack with other types than int
    assert(stack);
    assert(start_capacity > 0);

    ON_CANARY(
        stack->first_elem = CANARY;
        stack->last_elem = CANARY;
    )

    stack->capacity = start_capacity;
    stack->size = 0;

    stack->data = (type_t*)calloc(RealSizeStack(stack->capacity, CNT_CANARIES), sizeof(type_t));

    if (stack->data == NULL) {
        STACK_CHECK_ERRORS(NOT_ENOUGH_MEMORY);
    }

    init_with_poisons(stack->data + OffsetDueCanaries(CNT_CANARIES), (size_t)stack->capacity);

    ON_CANARY(SettingCanariesToBegin(stack->data));
    ON_CANARY(SettingCanariesToEnd(stack->data, stack->capacity));

    ON_HASH(SetHash(stack));

    type_error_t code_error = VERIFY(stack);
    STACK_CHECK_ERRORS(code_error, free(stack->data));

    return code_error;
}

ON_DEBUG(
type_error_t StackCtorDebug(stack_t* stack, size_t start_capacity, debug_info_t my_var_info) {
    type_error_t code_error = StackCtor(stack, start_capacity);

    STACK_CHECK_ERRORS(VERIFY(stack));

    stack->debug_info = my_var_info;

    ON_HASH(SetHash(stack));

    STACK_CHECK_ERRORS(VERIFY(stack));

    return code_error;
}
)

type_error_t StackReSize(stack_t* stack, size_t old_capacity) {
    assert(stack);
    assert(old_capacity > 0);
    assert(stack->capacity > 0);

    type_t* temp_data = (type_t*)my_recalloc(stack->data, RealSizeStack(stack->capacity, CNT_CANARIES) * sizeof(type_t), 
                                                          RealSizeStack(old_capacity, CNT_CANARIES) * sizeof(type_t));

    if (temp_data == NULL) {
        STACK_CHECK_ERRORS(NOT_ENOUGH_MEMORY, free(temp_data););
    }

    stack->data = temp_data;

    if (stack->capacity > old_capacity) {
        
        init_with_poisons(stack->data + OffsetToNewElement(stack->size, CNT_CANARIES), stack->capacity - stack->size);

        // ON_DEBUG(fprintf(stderr, "MEOW I'm recalloc up, I do it, size %zu, capacity %zu\n", stack->size, stack->capacity));
    }
    else {
        stack->data = temp_data;

        // ON_DEBUG(fprintf(stderr, "MEOW I'm recalloc down, I do it, size %zu, capacity %zu\n", stack->size, stack->capacity);)
    }    

    ON_CANARY(SettingCanariesToEnd(stack->data, stack->capacity));

    ON_HASH(SetHash(stack));

    STACK_CHECK_ERRORS(VERIFY(stack));

    return STACK_SUCCESS;
}


type_error_t StackPush(stack_t* stack, type_t new_value) {
    type_error_t code_error = STACK_SUCCESS;

    STACK_CHECK_ERRORS(VERIFY(stack));

    if (stack->size >= stack->capacity) {
        size_t old_capacity = stack->capacity;
        stack->capacity *= REALLOC_COEFF;

        code_error = StackReSize(stack, old_capacity);

        STACK_CHECK_ERRORS(code_error);
    }

    *(stack->data + OffsetToNewElement(stack->size, CNT_CANARIES)) = new_value;
    stack->size++;

    ON_HASH(SetHash(stack));

    STACK_CHECK_ERRORS(VERIFY(stack));

    return code_error;
}

type_error_t StackPeek(stack_t* stack, type_t* peek_element) {
    type_error_t code_error = STACK_SUCCESS;

    STACK_CHECK_ERRORS(VERIFY(stack));

    *peek_element = *(stack->data + OffsetToLastElement(stack->size, CNT_CANARIES));

    STACK_CHECK_ERRORS(VERIFY(stack));

    return code_error;
}

type_error_t StackPop(stack_t* stack, type_t* deleted_value) {
    type_error_t code_error = STACK_SUCCESS;

    STACK_CHECK_ERRORS(VERIFY(stack));

    if (stack->size == 0) {
        STACK_CHECK_ERRORS(POP_EMPTY_STACK);
    }

    if (deleted_value) {
        StackPeek(stack, deleted_value);
    }

    *(stack->data + OffsetToLastElement(stack->size, CNT_CANARIES)) = DEFAULT_POISON;

    stack->size--;

    if (stack->size * (REALLOC_COEFF * REALLOC_COEFF) < stack->capacity && stack->size != 0) { // FIXME
        size_t old_capacity = stack->capacity;
        stack->capacity /= REALLOC_COEFF;

        code_error = StackReSize(stack, old_capacity);

        STACK_CHECK_ERRORS(code_error);
    }

    ON_HASH(SetHash(stack));

    STACK_CHECK_ERRORS(VERIFY(stack));

    return code_error;
}

type_error_t StackDtor(stack_t* stack) {
    type_error_t code_error = VERIFY(stack);

    ON_DEBUG (
    if (code_error != STACK_SUCCESS) {
        StackDump(stack, code_error, DUMP_VAR_INFO);
    }
    )

    free(stack->data);
    
    *stack = {};

    return code_error;
}

// NOTE: example of error managment in C
// NOTE: PROPAGATE_ERROR(string[0], free_string0)

// int foo() {
//     void *string[4] = {};
//
//     string[0] = malloc(1024);
//     if (!string[0])
//         goto free_string0;
//
//     string[1] = malloc(1024);
//     if (!string[1])
//         goto free_string1;
//
//     string[2] = malloc(1024);
//     if (!string[2])
//         goto free_string2;
//
//     string[3] = malloc(1024);
//     if (!string[3])
//         goto free_string3;
//
// free_string3:
//     free(string[3]);
//
// free_string2:
//     free(string[2]);
//
// free_string1:
//     free(string[1]);
//
// free_string0:
//     free(string[0]);
//
//     return 41;
// }



// look how beautiful can it be
// int foo() {
//     void *string[4] = {};
//
//     PROPAGATE_ERROR(string[0] = malloc(1024), 0);
//     PROPAGATE_ERROR(string[1] = malloc(1124), 1);
//     PROPAGATE_ERROR(string[2] = malloc(2224), 2);
//     PROPAGATE_ERROR(string[3] = malloc(3334), 3);
//
// _3: free(string[3]);
// _2: free(string[2]);
// _1: free(string[1]);
// _0: free(string[0]);
//
//     return 41;
// }
