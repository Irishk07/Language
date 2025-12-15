#include <stdint.h>

#include "check_errors.h"

#include "canary.h"
#include "hash.h"
#include "stack.h"
#include "variable_information.h"


type_error_t StackVerify(const stack_t* stack) {
    type_error_t code_error = STACK_SUCCESS;

    if (stack == NULL) {
        code_error |= NULL_POINTER_ON_STRUCT; 

        return code_error;
    }
            
    if (stack->data == NULL && stack->size != 0) code_error |= NULL_POINTER_ON_DATA; 
    if (stack->capacity > MAX_CAPACITY)          code_error |= TOO_BIG_CAPACITY;
    if (stack->size > MAX_CAPACITY)              code_error |= TOO_BIG_SIZE;
    if (stack->size > stack->capacity)           code_error |= SIZE_BIGGER_THAN_CAPACITY;

    if (stack->data != NULL) {
        for (size_t i = 0; i < stack->capacity; ++i) {
            if (i < stack->size && 
                *(stack->data + RealIndex(i, CNT_CANARIES)) == DEFAULT_POISON)              code_error |= STACK_DATA_IS_POISON;

            else if (i >= stack->size && 
                *(stack->data + RealIndex(i, CNT_CANARIES)) != DEFAULT_POISON)              code_error |= STACK_OVERFLOW;
        }

        ON_CANARY (
        if ((*(stack->data) != CANARY) || 
            (*(stack->data + OffsetToNewElement(stack->capacity, CNT_CANARIES)) != CANARY)) code_error |= CORRUPTED_DATA_CANARY;
        )
    }

    ON_CANARY (
    if ((stack->first_elem != CANARY) ||
        (stack->last_elem  != CANARY))                                                      code_error |= CORRUPTED_STRUCT_CANARY;
    )  
    
    ON_HASH (
    uint64_t right_hash_struct = stack->hash_struct;
    stack->hash_struct = 0;

    uint64_t right_hash_data   = stack->hash_data;

    if (right_hash_struct != StackHash(stack, sizeof(stack_t)))                                code_error |= CORRUPTED_STRUCT_HASH;
    if (right_hash_data   != StackHash(stack->data, 
                             RealSizeStack(stack->capacity, CNT_CANARIES) * sizeof(type_t))) code_error |= CORRUPTED_DATA_HASH;

    stack->hash_struct = right_hash_struct;
    )

    return code_error;
}

void StackPrintError(type_error_t code_error) {
    if (code_error == STACK_SUCCESS)            fprintf(stderr, "    Not error: all right\n");
    if (code_error & NULL_POINTER_ON_STRUCT)    fprintf(stderr, "    Error is: pointer on stack is NULL\n");
    if (code_error & NULL_POINTER_ON_DATA)      fprintf(stderr, "    Error is: pointer on data in stack is NULL\n");
    if (code_error & TOO_BIG_CAPACITY)          fprintf(stderr, "    Error is: capacity is too big\n");
    if (code_error & TOO_BIG_SIZE)              fprintf(stderr, "    Error is: size is too big\n");
    if (code_error & SIZE_BIGGER_THAN_CAPACITY) fprintf(stderr, "    Error is: size is bigger than capacity\n");
    if (code_error & NOT_ENOUGH_MEMORY)         fprintf(stderr, "    Error is: problems with allocation memory\n");
    if (code_error & POP_EMPTY_STACK)           fprintf(stderr, "    Error is: try to pop empty stack\n");
    if (code_error & STACK_DATA_IS_POISON)      fprintf(stderr, "    Error is: stack data is poison\n");
    if (code_error & STACK_OVERFLOW)            fprintf(stderr, "    Error is: stack overflow\n");
    if (code_error & CORRUPTED_DATA_CANARY)     fprintf(stderr, "    Error is: data canary is corrupted\n");
    if (code_error & CORRUPTED_STRUCT_CANARY)   fprintf(stderr, "    Error is: struct canary is corrupted\n");  
    if (code_error & CORRUPTED_DATA_HASH)       fprintf(stderr, "    Error is: data hash is corrupted\n");
    if (code_error & CORRUPTED_STRUCT_HASH)     fprintf(stderr, "    Error is: struct hash is corrupted\n");
}

void StackDump(const stack_t* stack, type_error_t code_error, int line, const char* function_name, const char* file_name) {
    fprintf(stderr, "%s <%s>[%p] ", "stack_t", TYPE_NAME, &stack);

    PRINT_DUMP_DEBUG_INFO(stack);
    
    fprintf(stderr, "\n    called at %s() %s:%d:\n", function_name, file_name, line);
    
    StackPrintError(code_error);
    
    fprintf(stderr, "    %s = %zu\n", "capacity", stack->capacity);
    fprintf(stderr, "    %s = %zu\n", "size", stack->size);

    ON_HASH(
    fprintf(stderr, "    %s = %lu\n", "hash_struct", stack->hash_struct);
    fprintf(stderr, "    %s = %lu\n", "hash_data", stack->hash_data);

    fprintf(stderr, "struct hash %lu\n", StackHash(stack, sizeof(*stack)));
    )

    fprintf(stderr, "    %s[%zu] = [%p] {\n", "data", stack->capacity, &(stack->data));

    ON_CANARY(fprintf(stderr, "    +[%d] = " TYPE_T_PRINTF_SPECIFIER " (%s)\n", -1, *(stack->data), "CANARY"));
    
    for (size_t i = 0; i < stack->capacity; ++i) {
        if (i >= stack->size) fprintf(stderr, "     [%zu] = " TYPE_T_PRINTF_SPECIFIER " (%s)\n", i, *(stack->data + RealIndex(i, CNT_CANARIES)), "POISON");
        else                  fprintf(stderr, "    *[%zu] = " TYPE_T_PRINTF_SPECIFIER "\n", i, *(stack->data + RealIndex(i, CNT_CANARIES)));
    }

    ON_CANARY(fprintf(stderr, "    +[%d] = " TYPE_T_PRINTF_SPECIFIER " (%s)\n", -1, *(stack->data + OffsetToNewElement(stack->capacity, CNT_CANARIES)), "CANARY"));

    fprintf(stderr, "   }\n}\n\n");
}