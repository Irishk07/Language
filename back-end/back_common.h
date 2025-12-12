#ifndef COMMON_H_
#define COMMON_H_

#define TYPE_T_PRINTF_SPECIFIER "%ld"

#ifdef ASSEMBLER
#define IF_ASSEMBLER(...) __VA_ARGS__
#else
#define IF_ASSEMBLER(...)
#endif // ASSEMBLER

#ifdef PROCESSOR
#define IF_PROCESSOR(...) __VA_ARGS__
#else
#define IF_PROCESSOR(...)
#endif // PROCESSOR

#include <stdio.h>
#include <stdint.h>

typedef long type_t;

typedef uint64_t type_error_t;

const int CNT_REGISTERS = 8;

enum code_comand {
    CMD_PUSH   = 1,
    CMD_POP    = 2,
    CMD_ADD    = 3,
    CMD_SUB    = 4,
    CMD_DIV    = 5,
    CMD_MUL    = 6,
    CMD_SQRT   = 7,
    CMD_POW    = 8,
    CMD_IN     = 9,
    CMD_OUT    = 10,
    CMD_HLT    = 11,
    CMD_JMP    = 12,
    CMD_JB     = 13, // <
    CMD_JBE    = 14, // <=
    CMD_JA     = 15, // >
    CMD_JAE    = 16, // >=
    CMD_JE     = 17, // ==
    CMD_JNE    = 18, // !=
    CMD_CALL   = 19,
    CMD_RET    = 20,
    CMD_PUSHMR = 21,
    CMD_POPMR  = 22,
    CMD_PUSHMN = 23,
    CMD_POPMN  = 24,
    CMD_PUSHR  = 33,
    CMD_POPR   = 42
};

enum type_arguments {
    NO_ARGUMENT      = 0,
    NUM_ARGUMENT     = 1,
    REG_ARGUMENT     = 2,
    LABEL_ARGUMENT   = 3,
    RAM_REG_ARGUMENT = 4,
    RAM_NUM_ARGUMENT = 5
};

enum processor_status {
    PROC_SUCCESS                  = 0,
    PROC_UNKNOWN_COMAND           = 1 << 0,
    PROC_DIVISION_BY_ZERO         = 1 << 1,
    PROC_OPEN_ERROR               = 1 << 2,
    PROC_CLOSE_ERROR              = 1 << 3,
    PROC_READ_ERROR               = 1 << 4,
    STACK_ERROR                   = 1 << 5,
    PROC_SQRT_NEGATIVE_NUM        = 1 << 6,
    PROC_STAT_ERROR               = 1 << 7,
    PROC_NOT_ENOUGH_MEMORY        = 1 << 8,
    PROC_NULL_POINTER_ON_FILE     = 1 << 9,
    PROC_NULL_POINTER_ON_DATA     = 1 << 10,
    PROC_NULL_POINTER_ON_STRUCT   = 1 << 11,
    PROC_CNT_COMMANDS_IS_NEGATIVE = 1 << 12,
    PROC_WRONG_BYTE_CODE          = 1 << 13,
    PROC_INVALID_REGISTER         = 1 << 14,
    PROC_EXPECTS_ARG              = 1 << 15,
    PROC_EXPECTS_HLT              = 1 << 16
};


struct About_text {
    char *text = NULL;
    const char *text_name = NULL;
    int cnt_strok = 0;
    int text_size = 0;
    char** pointer_on_text = NULL;
};

struct Processor;

struct About_commands {
    const char* name;
    type_t code;
    int type_argument;
    type_t argument;
    IF_ASSEMBLER(unsigned long hash);
    IF_PROCESSOR(processor_status (*function) (Processor* processor));
};

IF_ASSEMBLER(unsigned long hash_djb2(const char *str));

IF_PROCESSOR(
processor_status do_push(Processor* processor);

processor_status do_pop(Processor* processor);

processor_status do_add(Processor* processor);

processor_status do_sub(Processor* processor);

processor_status do_div(Processor* processor);

processor_status do_mul(Processor* processor);

processor_status do_sqrt(Processor* processor);

processor_status do_pow(Processor* processor);

processor_status do_in(Processor* processor);

processor_status do_popr(Processor* processor);

processor_status do_pushr(Processor* processor);

processor_status do_out(Processor* processor);

processor_status do_jmp(Processor* processor);

processor_status do_jb(Processor* processor);

processor_status do_jbe(Processor* processor);

processor_status do_ja(Processor* processor);

processor_status do_jae(Processor* processor);

processor_status do_je(Processor* processor);

processor_status do_jne(Processor* processor);

processor_status do_call(Processor* processor);

processor_status do_ret(Processor* processor);

processor_status do_pushmr(Processor* processor);

processor_status do_popmr(Processor* processor);

processor_status do_pushmn(Processor* processor);

processor_status do_popmn(Processor* processor);
)


const About_commands about_commands [] = {
    {.name = "PUSH", .code = CMD_PUSH,  .type_argument = NUM_ARGUMENT,     .argument = 0 IF_ASSEMBLER(, .hash = hash_djb2((const char*)"PUSH"))  IF_PROCESSOR(, .function = &do_push)},
    {.name = "POP",  .code = CMD_POP,   .type_argument = NO_ARGUMENT,      .argument = 0 IF_ASSEMBLER(, .hash = hash_djb2((const char*)"POP"))   IF_PROCESSOR(, .function = &do_pop)},
    {.name = "ADD",  .code = CMD_ADD,   .type_argument = NO_ARGUMENT,      .argument = 0 IF_ASSEMBLER(, .hash = hash_djb2((const char*)"ADD"))   IF_PROCESSOR(, .function = &do_add)},
    {.name = "SUB",  .code = CMD_SUB,   .type_argument = NO_ARGUMENT,      .argument = 0 IF_ASSEMBLER(, .hash = hash_djb2((const char*)"SUB"))   IF_PROCESSOR(, .function = &do_sub)},
    {.name = "DIV",  .code = CMD_DIV,   .type_argument = NO_ARGUMENT,      .argument = 0 IF_ASSEMBLER(, .hash = hash_djb2((const char*)"DIV"))   IF_PROCESSOR(, .function = &do_div)},
    {.name = "MUL",  .code = CMD_MUL,   .type_argument = NO_ARGUMENT,      .argument = 0 IF_ASSEMBLER(, .hash = hash_djb2((const char*)"MUL"))   IF_PROCESSOR(, .function = &do_mul)},
    {.name = "SQRT", .code = CMD_SQRT,  .type_argument = NO_ARGUMENT,      .argument = 0 IF_ASSEMBLER(, .hash = hash_djb2((const char*)"SQRT"))  IF_PROCESSOR(, .function = &do_sqrt)},
    {.name = "POW",  .code = CMD_POW,   .type_argument = NO_ARGUMENT,      .argument = 0 IF_ASSEMBLER(, .hash = hash_djb2((const char*)"POW"))   IF_PROCESSOR(, .function = &do_pow)},
    {.name = "IN",   .code = CMD_IN,    .type_argument = NO_ARGUMENT,      .argument = 0 IF_ASSEMBLER(, .hash = hash_djb2((const char*)"IN"))    IF_PROCESSOR(, .function = &do_in)},
    {.name = "OUT",  .code = CMD_OUT,   .type_argument = NO_ARGUMENT,      .argument = 0 IF_ASSEMBLER(, .hash = hash_djb2((const char*)"OUT"))   IF_PROCESSOR(, .function = &do_out)},
    {.name = "HLT",  .code = CMD_HLT,   .type_argument = NO_ARGUMENT,      .argument = 0 IF_ASSEMBLER(, .hash = hash_djb2((const char*)"HLT"))   IF_PROCESSOR(, .function = NULL)},
    {.name = "JMP",  .code = CMD_JMP,   .type_argument = LABEL_ARGUMENT,   .argument = 0 IF_ASSEMBLER(, .hash = hash_djb2((const char*)"JMP"))   IF_PROCESSOR(, .function = &do_jmp)},
    {.name = "JB",   .code = CMD_JB,    .type_argument = LABEL_ARGUMENT,   .argument = 0 IF_ASSEMBLER(, .hash = hash_djb2((const char*)"JB"))    IF_PROCESSOR(, .function = &do_jb)},
    {.name = "JBE",  .code = CMD_JBE,   .type_argument = LABEL_ARGUMENT,   .argument = 0 IF_ASSEMBLER(, .hash = hash_djb2((const char*)"JBE"))   IF_PROCESSOR(, .function = &do_jbe)},
    {.name = "JA",   .code = CMD_JA,    .type_argument = LABEL_ARGUMENT,   .argument = 0 IF_ASSEMBLER(, .hash = hash_djb2((const char*)"JA"))    IF_PROCESSOR(, .function = &do_ja)},
    {.name = "JAE",  .code = CMD_JAE,   .type_argument = LABEL_ARGUMENT,   .argument = 0 IF_ASSEMBLER(, .hash = hash_djb2((const char*)"JAE"))   IF_PROCESSOR(, .function = &do_jae)},
    {.name = "JE",   .code = CMD_JE,    .type_argument = LABEL_ARGUMENT,   .argument = 0 IF_ASSEMBLER(, .hash = hash_djb2((const char*)"JE"))    IF_PROCESSOR(, .function = &do_je)},
    {.name = "JNE",  .code = CMD_JNE,   .type_argument = LABEL_ARGUMENT,   .argument = 0 IF_ASSEMBLER(, .hash = hash_djb2((const char*)"JNE"))   IF_PROCESSOR(, .function = &do_jne)},
    {.name = "CALL", .code = CMD_CALL,  .type_argument = LABEL_ARGUMENT,   .argument = 0 IF_ASSEMBLER(, .hash = hash_djb2((const char*)"CALL"))  IF_PROCESSOR(, .function = &do_call)},
    {.name = "RET",  .code = CMD_RET,   .type_argument = NO_ARGUMENT,      .argument = 0 IF_ASSEMBLER(, .hash = hash_djb2((const char*)"RET"))   IF_PROCESSOR(, .function = &do_ret)},
    {.name = "PUSHR",.code = CMD_PUSHR, .type_argument = REG_ARGUMENT,     .argument = 0 IF_ASSEMBLER(, .hash = hash_djb2((const char*)"PUSHR")) IF_PROCESSOR(, .function = &do_pushr)},
    {.name = "POPR", .code = CMD_POPR,  .type_argument = REG_ARGUMENT,     .argument = 0 IF_ASSEMBLER(, .hash = hash_djb2((const char*)"POPR"))  IF_PROCESSOR(, .function = &do_popr)},
    {.name = "PUSHMR",.code = CMD_PUSHMR, .type_argument = RAM_REG_ARGUMENT, .argument = 0 IF_ASSEMBLER(, .hash = hash_djb2((const char*)"PUSHMR")) IF_PROCESSOR(, .function = &do_pushmr)},
    {.name = "POPMR", .code = CMD_POPMR,  .type_argument = RAM_REG_ARGUMENT, .argument = 0 IF_ASSEMBLER(, .hash = hash_djb2((const char*)"POPMR"))  IF_PROCESSOR(, .function = &do_popmr)},
    {.name = "PUSHMR",.code = CMD_PUSHMN, .type_argument = RAM_NUM_ARGUMENT, .argument = 0 IF_ASSEMBLER(, .hash = hash_djb2((const char*)"PUSHMN")) IF_PROCESSOR(, .function = &do_pushmn)},
    {.name = "POPMR", .code = CMD_POPMN,  .type_argument = RAM_NUM_ARGUMENT, .argument = 0 IF_ASSEMBLER(, .hash = hash_djb2((const char*)"POPMN"))  IF_PROCESSOR(, .function = &do_popmn)}
};

const size_t SIZE_ABOUT_COMMANDS = sizeof(about_commands) / sizeof(about_commands[0]);

#endif // COMMON_H_