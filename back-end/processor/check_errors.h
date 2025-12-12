#ifndef CHECK_ERRORS_H_
#define CHECK_ERRORS_H_

#include "stack.h"

#ifdef DEBUG
#define VERIFY(stack) StackVerify(stack)
#else // NOT DEBUG
#define VERIFY(stack) STACK_SUCCESS
#endif // DEBUG


type_error_t StackVerify(const stack_t* stack);

void StackPrintError(type_error_t code_error);

void StackDump(const stack_t* stack, type_error_t code_error, int line, const char* function_name, const char* file_name);


#endif //CHECK_ERRORS_H_