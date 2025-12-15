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
    CMD_PUSHR  = 25,
    CMD_POPR   = 26,
    CMD_LN     = 27,
    CMD_LOG    = 28,
    CMD_SIN    = 29,
    CMD_COS    = 30,
    CMD_TG     = 31,
    CMD_CTG    = 32,
    CMD_ARCSIN = 33,
    CMD_ARCCOS = 34,
    CMD_ARCTG  = 35,
    CMD_ARCCTG = 36,
    CMD_SH     = 37,
    CMD_CH     = 38,  
    CMD_TH     = 39,
    CMD_CTH    = 40
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
    PROC_EXPECTS_HLT              = 1 << 16,
    PROC_LN_NEGATIVE_NUM          = 1 << 17,
    PROC_LOG_NEGATIVE_NUM         = 1 << 18,
    PROC_CTG_NEGATIVE_NUM         = 1 << 19,
    PROC_ARCSIN_NEGATIVE_NUM      = 1 << 20,
    PROC_ARCCOS_NEGATIVE_NUM      = 1 << 21,
    PROC_CTH_NEGATIVE_NUM         = 1 << 22
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

processor_status do_ln(Processor* processor);

processor_status do_log(Processor* processor);

processor_status do_sin(Processor* processor);

processor_status do_cos(Processor* processor);

processor_status do_tg(Processor* processor);

processor_status do_ctg(Processor* processor);

processor_status do_arcsin(Processor* processor);

processor_status do_arccos(Processor* processor);

processor_status do_arctg(Processor* processor);

processor_status do_arcctg(Processor* processor);

processor_status do_sh(Processor* processor);

processor_status do_ch(Processor* processor);

processor_status do_th(Processor* processor);

processor_status do_cth(Processor* processor);
)


const About_commands about_commands [] = {
    {.name = "PUSH",  .code = CMD_PUSH,   .type_argument = NUM_ARGUMENT,     .argument = 0 IF_ASSEMBLER(, .hash = hash_djb2("PUSH"))   IF_PROCESSOR(, .function = &do_push)},
    {.name = "POP",   .code = CMD_POP,    .type_argument = NO_ARGUMENT,      .argument = 0 IF_ASSEMBLER(, .hash = hash_djb2("POP"))    IF_PROCESSOR(, .function = &do_pop)},
    {.name = "ADD",   .code = CMD_ADD,    .type_argument = NO_ARGUMENT,      .argument = 0 IF_ASSEMBLER(, .hash = hash_djb2("ADD"))    IF_PROCESSOR(, .function = &do_add)},
    {.name = "SUB",   .code = CMD_SUB,    .type_argument = NO_ARGUMENT,      .argument = 0 IF_ASSEMBLER(, .hash = hash_djb2("SUB"))    IF_PROCESSOR(, .function = &do_sub)},
    {.name = "DIV",   .code = CMD_DIV,    .type_argument = NO_ARGUMENT,      .argument = 0 IF_ASSEMBLER(, .hash = hash_djb2("DIV"))    IF_PROCESSOR(, .function = &do_div)},
    {.name = "MUL",   .code = CMD_MUL,    .type_argument = NO_ARGUMENT,      .argument = 0 IF_ASSEMBLER(, .hash = hash_djb2("MUL"))    IF_PROCESSOR(, .function = &do_mul)},
    {.name = "SQRT",  .code = CMD_SQRT,   .type_argument = NO_ARGUMENT,      .argument = 0 IF_ASSEMBLER(, .hash = hash_djb2("SQRT"))   IF_PROCESSOR(, .function = &do_sqrt)},
    {.name = "POW",   .code = CMD_POW,    .type_argument = NO_ARGUMENT,      .argument = 0 IF_ASSEMBLER(, .hash = hash_djb2("POW"))    IF_PROCESSOR(, .function = &do_pow)},
    {.name = "IN",    .code = CMD_IN,     .type_argument = NO_ARGUMENT,      .argument = 0 IF_ASSEMBLER(, .hash = hash_djb2("IN"))     IF_PROCESSOR(, .function = &do_in)},
    {.name = "OUT",   .code = CMD_OUT,    .type_argument = NO_ARGUMENT,      .argument = 0 IF_ASSEMBLER(, .hash = hash_djb2("OUT"))    IF_PROCESSOR(, .function = &do_out)},
    {.name = "HLT",   .code = CMD_HLT,    .type_argument = NO_ARGUMENT,      .argument = 0 IF_ASSEMBLER(, .hash = hash_djb2("HLT"))    IF_PROCESSOR(, .function = NULL)},
    {.name = "JMP",   .code = CMD_JMP,    .type_argument = LABEL_ARGUMENT,   .argument = 0 IF_ASSEMBLER(, .hash = hash_djb2("JMP"))    IF_PROCESSOR(, .function = &do_jmp)},
    {.name = "JB",    .code = CMD_JB,     .type_argument = LABEL_ARGUMENT,   .argument = 0 IF_ASSEMBLER(, .hash = hash_djb2("JB"))     IF_PROCESSOR(, .function = &do_jb)},
    {.name = "JBE",   .code = CMD_JBE,    .type_argument = LABEL_ARGUMENT,   .argument = 0 IF_ASSEMBLER(, .hash = hash_djb2("JBE"))    IF_PROCESSOR(, .function = &do_jbe)},
    {.name = "JA",    .code = CMD_JA,     .type_argument = LABEL_ARGUMENT,   .argument = 0 IF_ASSEMBLER(, .hash = hash_djb2("JA"))     IF_PROCESSOR(, .function = &do_ja)},
    {.name = "JAE",   .code = CMD_JAE,    .type_argument = LABEL_ARGUMENT,   .argument = 0 IF_ASSEMBLER(, .hash = hash_djb2("JAE"))    IF_PROCESSOR(, .function = &do_jae)},
    {.name = "JE",    .code = CMD_JE,     .type_argument = LABEL_ARGUMENT,   .argument = 0 IF_ASSEMBLER(, .hash = hash_djb2("JE"))     IF_PROCESSOR(, .function = &do_je)},
    {.name = "JNE",   .code = CMD_JNE,    .type_argument = LABEL_ARGUMENT,   .argument = 0 IF_ASSEMBLER(, .hash = hash_djb2("JNE"))    IF_PROCESSOR(, .function = &do_jne)},
    {.name = "CALL",  .code = CMD_CALL,   .type_argument = LABEL_ARGUMENT,   .argument = 0 IF_ASSEMBLER(, .hash = hash_djb2("CALL"))   IF_PROCESSOR(, .function = &do_call)},
    {.name = "RET",   .code = CMD_RET,    .type_argument = NO_ARGUMENT,      .argument = 0 IF_ASSEMBLER(, .hash = hash_djb2("RET"))    IF_PROCESSOR(, .function = &do_ret)},
    {.name = "PUSHR", .code = CMD_PUSHR,  .type_argument = REG_ARGUMENT,     .argument = 0 IF_ASSEMBLER(, .hash = hash_djb2("PUSHR"))  IF_PROCESSOR(, .function = &do_pushr)},
    {.name = "POPR",  .code = CMD_POPR,   .type_argument = REG_ARGUMENT,     .argument = 0 IF_ASSEMBLER(, .hash = hash_djb2("POPR"))   IF_PROCESSOR(, .function = &do_popr)},
    {.name = "PUSHMR",.code = CMD_PUSHMR, .type_argument = RAM_REG_ARGUMENT, .argument = 0 IF_ASSEMBLER(, .hash = hash_djb2("PUSHMR")) IF_PROCESSOR(, .function = &do_pushmr)},
    {.name = "POPMR", .code = CMD_POPMR,  .type_argument = RAM_REG_ARGUMENT, .argument = 0 IF_ASSEMBLER(, .hash = hash_djb2("POPMR"))  IF_PROCESSOR(, .function = &do_popmr)},
    {.name = "PUSHMN",.code = CMD_PUSHMN, .type_argument = RAM_NUM_ARGUMENT, .argument = 0 IF_ASSEMBLER(, .hash = hash_djb2("PUSHMN")) IF_PROCESSOR(, .function = &do_pushmn)},
    {.name = "POPMN", .code = CMD_POPMN,  .type_argument = RAM_NUM_ARGUMENT, .argument = 0 IF_ASSEMBLER(, .hash = hash_djb2("POPMN"))  IF_PROCESSOR(, .function = &do_popmn)},
    {.name = "LN",    .code = CMD_LN,     .type_argument = NO_ARGUMENT,      .argument = 0 IF_ASSEMBLER(, .hash = hash_djb2("LN"))     IF_PROCESSOR(, .function = &do_ln)},
    {.name = "LOG",   .code = CMD_LOG,    .type_argument = NO_ARGUMENT,      .argument = 0 IF_ASSEMBLER(, .hash = hash_djb2("LOG"))    IF_PROCESSOR(, .function = &do_log)},
    {.name = "SIN",   .code = CMD_SIN,    .type_argument = NO_ARGUMENT,      .argument = 0 IF_ASSEMBLER(, .hash = hash_djb2("SIN"))    IF_PROCESSOR(, .function = &do_sin)},
    {.name = "COS",   .code = CMD_COS,    .type_argument = NO_ARGUMENT,      .argument = 0 IF_ASSEMBLER(, .hash = hash_djb2("COS"))    IF_PROCESSOR(, .function = &do_cos)},
    {.name = "TG",    .code = CMD_TG,     .type_argument = NO_ARGUMENT,      .argument = 0 IF_ASSEMBLER(, .hash = hash_djb2("TG"))     IF_PROCESSOR(, .function = &do_tg)},
    {.name = "CTG",   .code = CMD_CTG,    .type_argument = NO_ARGUMENT,      .argument = 0 IF_ASSEMBLER(, .hash = hash_djb2("CTG"))    IF_PROCESSOR(, .function = &do_ctg)},
    {.name = "ARCSIN",.code = CMD_ARCSIN, .type_argument = NO_ARGUMENT,      .argument = 0 IF_ASSEMBLER(, .hash = hash_djb2("ARCSIN")) IF_PROCESSOR(, .function = &do_arcsin)},
    {.name = "ARCCOS",.code = CMD_ARCCOS, .type_argument = NO_ARGUMENT,      .argument = 0 IF_ASSEMBLER(, .hash = hash_djb2("ARCCOS")) IF_PROCESSOR(, .function = &do_arccos)},
    {.name = "ARCTG", .code = CMD_ARCTG,  .type_argument = NO_ARGUMENT,      .argument = 0 IF_ASSEMBLER(, .hash = hash_djb2("ARCTG"))  IF_PROCESSOR(, .function = &do_arctg)},
    {.name = "ARCCTG",.code = CMD_ARCCTG, .type_argument = NO_ARGUMENT,      .argument = 0 IF_ASSEMBLER(, .hash = hash_djb2("ARCCTG")) IF_PROCESSOR(, .function = &do_arcctg)},
    {.name = "SH",    .code = CMD_SH,     .type_argument = NO_ARGUMENT,      .argument = 0 IF_ASSEMBLER(, .hash = hash_djb2("SH"))     IF_PROCESSOR(, .function = &do_sh)},
    {.name = "CH",    .code = CMD_CH,     .type_argument = NO_ARGUMENT,      .argument = 0 IF_ASSEMBLER(, .hash = hash_djb2("CH"))     IF_PROCESSOR(, .function = &do_ch)},
    {.name = "TH",    .code = CMD_TH,     .type_argument = NO_ARGUMENT,      .argument = 0 IF_ASSEMBLER(, .hash = hash_djb2("TH"))     IF_PROCESSOR(, .function = &do_th)},
    {.name = "CTH",   .code = CMD_CTH,    .type_argument = NO_ARGUMENT,      .argument = 0 IF_ASSEMBLER(, .hash = hash_djb2("CTH"))    IF_PROCESSOR(, .function = &do_cth)}
};

const size_t SIZE_ABOUT_COMMANDS = sizeof(about_commands) / sizeof(about_commands[0]);


#endif // COMMON_H_