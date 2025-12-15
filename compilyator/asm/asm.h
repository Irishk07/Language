#ifndef ASM_H_
#define ASM_H_

#include <stdio.h>

#include "../proc_common.h"

#define CHECK_AND_RETURN_ERRORS_ASM(error, ...)                          \
        if ((error) != ASM_SUCCESS) {                                    \
            fprintf(stderr, "Error is: %d, line %d\n", error, __LINE__); \
            __VA_ARGS__;                                                 \
            return (error);                                              \
        }


const int LEN_NAME_REGISTER          = 3;
const int CNT_LABELS                 = 10;
const int MAX_CNT_COMMANDS           = 128;

enum assembler_status {
    ASM_SUCCESS                      = 0,
    ASM_EXPECTS_NUMBER               = 1 << 0,
    ASM_EXPECTS_HLT                  = 1 << 1,
    ASM_NOT_ENOUGH_MEMORY            = 1 << 2,
    ASM_UNKNOWN_COMAND               = 1 << 3,
    ASM_READ_ERROR                   = 1 << 4,
    ASM_OPEN_ERROR                   = 1 << 5,
    ASM_CLOSE_ERROR                  = 1 << 6,
    ASM_STAT_ERROR                   = 1 << 7,
    ASM_EXPECTS_REGISTER             = 1 << 8,
    ASM_NULL_POINTER_ON_STRUCT       = 1 << 9,
    ASM_NULL_POINTER_ON_DATA         = 1 << 10,
    ASM_NULL_POINTER_ON_NAME_OF_FILE = 1 << 11,
    ASM_EXPECTS_JUMP_ARG             = 1 << 12,
    ASM_NOT_FOUND_LABEL              = 1 << 13,
    ASM_EXPECTS_ARG                  = 1 << 14,
    ASM_DOUBLE_LABEL                 = 1 << 15
};

enum status_cmp {
    DIFFERENT     = 0,
    EQUAL         = 1,
    ASM_CMP_ERROR = -1
};

enum number_of_compile {
    FIRST_COMPILE  = 1,
    SECOND_COMPILE = 2
};


struct Byte_code_data {
    type_t* data;
    size_t size;
};

struct About_labels {
    unsigned long hash;
    type_t index;
};

struct About_register {
    const char* register_name;
    unsigned long hash;
};

struct Asm_about_commands {
    const char* name;
    type_t code;
    int type_argument;
    type_t argument;
    unsigned long hash;
};

struct Assembler {
    Byte_code_data byte_code_data;
    About_text about_text;
    About_labels about_labels[CNT_LABELS];
    int cnt_current_label;
    Asm_about_commands asm_about_commands[SIZE_ABOUT_COMMANDS];
};


assembler_status AsmCtor(Assembler* assembler, const char* name_comands_file);

void AsmInitAboutCommands(Assembler* assembler);

void AsmInitLabels(Assembler* assembler);

number_of_compile FirstOrSecondCompile(const Assembler* assembler);

assembler_status AsmVerify(const Assembler* assembler);

int qsort_commands_comparator(const void* num1, const void* num2);

int qsort_register_comparator(const void* param1, const void* param2);

int qsort_label_comparator(const void* param1, const void* param2);

int bsearch_commands_comparator(const void* param1, const void* param2);

int bsearch_register_comparator(const void* param1, const void* param2);

assembler_status Assemblirovanie(Assembler* assembler);

void InsertLabel(Assembler* assembler, char* string);

status_cmp FindCommand(Assembler* assembler, char* string, Asm_about_commands* current_command);

void CheckPushPopCommands(Assembler* assembler, char* string, Asm_about_commands* current_command);

void FillCommand(Assembler* assembler, Asm_about_commands* current_command);

assembler_status PassArgs(Assembler* assembler, Asm_about_commands* current_command, char* string);

assembler_status GetFillArgNum(Assembler* assembler, Asm_about_commands* current_command, char* string);

assembler_status GetFillArgReg(Assembler* assembler, Asm_about_commands* current_command, char* string, int type_argument);

status_cmp CheckRegister(char* string, int type_argument, type_t* code);

assembler_status GetFillArgJump(Assembler* assembler, Asm_about_commands* current_command, char* string);

void FillListingFile(char* pointer_on_command, Asm_about_commands* current_command, FILE* listing_file, int type_argument);

assembler_status PrintfByteCode(Assembler* assembler);

assembler_status CheckDoubleLabels(Assembler* assembler);

assembler_status CreatByteCodeData(Assembler* assembler);

assembler_status CreateExeFile(const Assembler* assembler, const char* name_byte_code_file);

assembler_status AsmDtor(Assembler* assembler);


#endif //ASM_H_
