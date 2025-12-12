#ifndef PROCESSOR_H_
#define PROCESSOR_H_

#include "stack.h"
#include "stdio.h"

#define CHECK_AND_RETURN_ERRORS_STACK(error, ...) \
        if ((error) != STACK_SUCCESS) {           \
            __VA_ARGS__;                          \
            return STACK_ERROR;                   \
        }

#define CHECK_AND_RETURN_ERRORS_PROC(error, ...)                     \
        if ((error) != PROC_SUCCESS) {                               \
            __VA_ARGS__;                                             \
            fprintf(stderr, "Line %d, error %d\n", __LINE__, error); \
            /* ProcDump(processor, error, DUMP_VAR_INFO); */         \
            return (error);                                          \
        }


const int SIZE_RAM = 100;
const int MAX_CNT_COMMANDS = 64;


struct Proc_about_commands {
    const char* name;
    processor_status (*function) (Processor* processor);
};

struct Processor {
    stack_t stack;
    stack_t return_stack;
    size_t programm_cnt = 0;
    size_t cnt_commands = 0;
    type_t registers[CNT_REGISTERS]; // RAX RBX RCX RDX REX RFX RGX RHX
    About_text about_text;
    Proc_about_commands proc_about_commands[MAX_CNT_COMMANDS];
    int ram[SIZE_RAM] = {};
};

void ProcInitAboutCommands(Processor* processor);

processor_status ProcCtor(Processor* processor, const char* file_name);

processor_status SPU(Processor* processor);

processor_status ProcVerify(const Processor* processor);

void ProcDump(const Processor* processor, type_error_t code_error, int line, const char* function_name, const char* file_name);

processor_status ProcDtor(Processor* processor);

void draw_ram(Processor* processor);


#endif //PROCESSOR_H_