#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "asm.h"

#include "../back_common.h"
#include "onegin.h"


About_register about_register [] {
    {.register_name = "RAX",   .hash = hash_djb2((const char*)"RAX")},
    {.register_name = "[RAX]", .hash = hash_djb2((const char*)"[RAX]")},
    {.register_name = "RBX",   .hash = hash_djb2((const char*)"RBX")},
    {.register_name = "[RBX]", .hash = hash_djb2((const char*)"[RBX]")},
    {.register_name = "RCX",   .hash = hash_djb2((const char*)"RCX")},
    {.register_name = "[RCX]", .hash = hash_djb2((const char*)"[RCX]")},
    {.register_name = "RDX",   .hash = hash_djb2((const char*)"RDX")},
    {.register_name = "[RDX]", .hash = hash_djb2((const char*)"[RDX]")},
    {.register_name = "REX",   .hash = hash_djb2((const char*)"REX")},
    {.register_name = "[REX]", .hash = hash_djb2((const char*)"[REX]")},
    {.register_name = "RFX",   .hash = hash_djb2((const char*)"RFX")},
    {.register_name = "[RFX]", .hash = hash_djb2((const char*)"[RFX]")},
    {.register_name = "RGX",   .hash = hash_djb2((const char*)"RGX")},
    {.register_name = "[RGX]", .hash = hash_djb2((const char*)"[RGX]")},
    {.register_name = "RHX",   .hash = hash_djb2((const char*)"RHX")},
    {.register_name = "[RHX]", .hash = hash_djb2((const char*)"[RHX]")}
};


assembler_status AsmCtor(Assembler* assembler, const char* name_commands_file) {
    assert(assembler);
    assert(name_commands_file);

    assembler->about_text.text_name = name_commands_file;
    assembler->byte_code_data.size  = 0;

    AsmInitLabels(assembler);

    AsmInitAboutCommands(assembler);

    CHECK_AND_RETURN_ERRORS_ASM(OneginReadFile(assembler),      free(assembler->byte_code_data.data));

    CHECK_AND_RETURN_ERRORS_ASM(DivisionIntoCommands(assembler));

    CHECK_AND_RETURN_ERRORS_ASM(AsmVerify(assembler));

    qsort(assembler->asm_about_commands, sizeof(assembler->asm_about_commands) / sizeof(assembler->asm_about_commands[0]), 
          sizeof(Asm_about_commands), &qsort_commands_comparator);
    qsort(about_register, sizeof(about_register) / sizeof(about_register[0]), sizeof(About_register), &qsort_register_comparator);

    return ASM_SUCCESS;
}

void AsmInitLabels(Assembler* assembler) {
    assert(assembler);

    for (int i = 0; i < CNT_LABELS; ++i) {
        assembler->about_labels[i].hash  =  0;
        assembler->about_labels[i].index = -1;
    }
}

void AsmInitAboutCommands(Assembler* assembler) {
    assert(assembler);

    for (size_t i = 0; i < SIZE_ABOUT_COMMANDS; ++i) {
        assembler->asm_about_commands[i] = {.name = about_commands[i].name, .code = about_commands[i].code,
                                            .type_argument = about_commands[i].type_argument, .argument = about_commands[i].argument,
                                            .hash = about_commands[i].hash};
    }
}

number_of_compile FirstOrSecondCompile(const Assembler* assembler) {
    if (assembler->byte_code_data.data == NULL) 
        return FIRST_COMPILE;

    return SECOND_COMPILE;
}

assembler_status AsmVerify(const Assembler* assembler) {
    if (assembler == NULL) {
        return ASM_NULL_POINTER_ON_STRUCT;
    }

    if (FirstOrSecondCompile(assembler) == SECOND_COMPILE && assembler->byte_code_data.data == NULL) {
        return ASM_NULL_POINTER_ON_DATA;
    }

    if (assembler->about_text.text_name == NULL) {
        return ASM_NULL_POINTER_ON_NAME_OF_FILE;
    }

    return ASM_SUCCESS;
}

unsigned long hash_djb2(const char *str) {
    assert(str);

    unsigned long hash = 5381;
    int c = 0;

    while ((c = *(str++)) != '\0') {
        hash = ((hash << 5) + hash) + (unsigned long)c; /* hash * 33 + c */
    }

    return hash;
}


#define DO_QSORT_COMPARATOR(name, type)                                          \
                                                                                 \
int qsort_ ## name ## _comparator(const void* param1, const void* param2) {      \
    const struct type* struct1 = (const struct type*)param1;                     \
    const struct type* struct2 = (const struct type*)param2;                     \
                                                                                 \
    if (struct1->hash < struct2->hash) return -1;                                \
                                                                                 \
    if (struct1->hash > struct2->hash) return 1;                                 \
                                                                                 \
    return 0;                                                                    \
}

DO_QSORT_COMPARATOR(commands, Asm_about_commands)
DO_QSORT_COMPARATOR(register, About_register)
DO_QSORT_COMPARATOR(label,    About_labels)

#undef DO_QSORT_COMPARATOR

#define DO_BSEARCH_COMPARATOR(name, type)                                      \
                                                                               \
int bsearch_ ## name ## _comparator(const void* param1, const void* param2) {  \
    unsigned long search_hash = *(const unsigned long*)param1;                 \
    const struct type* struct1 = (const struct type*)param2;                   \
                                                                               \
    if (search_hash < struct1->hash) return -1;                                \
                                                                               \
    if (search_hash > struct1->hash) return 1;                                 \
                                                                               \
    return 0;                                                                  \
}

DO_BSEARCH_COMPARATOR(commands, Asm_about_commands)
DO_BSEARCH_COMPARATOR(register, About_register)

#undef DO_BSEARCH_COMPARATOR


assembler_status Assemblirovanie(Assembler* assembler) {
    CHECK_AND_RETURN_ERRORS_ASM(AsmVerify(assembler));

    assembler->cnt_current_label = 0;

    FILE* listing_file = NULL;
    if (FirstOrSecondCompile(assembler) == SECOND_COMPILE) {
        listing_file = fopen("listing_file.txt", "w");

        if (listing_file == NULL)
            CHECK_AND_RETURN_ERRORS_ASM(ASM_OPEN_ERROR);
    }

    bool find_cmd_hlt = false;

    for (int i = 0; i < assembler->about_text.cnt_strok; ++i) {
        CHECK_AND_RETURN_ERRORS_ASM(AsmVerify(assembler));

        if (strlen(assembler->about_text.pointer_on_text[i]) == 0) {
            continue;
        }

        if (assembler->about_text.pointer_on_text[i][0] == ':') {
            if (FirstOrSecondCompile(assembler) == FIRST_COMPILE)
                InsertLabel(assembler, assembler->about_text.pointer_on_text[i]);
            continue;
        }

        Asm_about_commands current_command = {};
        if (!FindCommand(assembler, assembler->about_text.pointer_on_text[i], &current_command))
            CHECK_AND_RETURN_ERRORS_ASM(ASM_UNKNOWN_COMAND);

        if (current_command.hash == hash_djb2("PUSH") || current_command.hash == hash_djb2("POP"))
            CheckPushPopCommands(assembler, assembler->about_text.pointer_on_text[i + 1], &current_command);

        FillCommand(assembler, &current_command);

        if (current_command.code == CMD_HLT) 
            find_cmd_hlt = true;

        if (current_command.type_argument != NO_ARGUMENT)
            CHECK_AND_RETURN_ERRORS_ASM(PassArgs(assembler, &current_command, assembler->about_text.pointer_on_text[++i]));

        if (FirstOrSecondCompile(assembler) == SECOND_COMPILE)
            FillListingFile(assembler->about_text.pointer_on_text[i], &current_command, listing_file, current_command.type_argument);

        CHECK_AND_RETURN_ERRORS_ASM(AsmVerify(assembler));
    }

    if (!find_cmd_hlt)
        CHECK_AND_RETURN_ERRORS_ASM(ASM_EXPECTS_HLT);

    PrintfByteCode(assembler);

    if (FirstOrSecondCompile(assembler) == SECOND_COMPILE) {
        if (fclose(listing_file) == EOF)
            CHECK_AND_RETURN_ERRORS_ASM(ASM_CLOSE_ERROR,    perror("Error is:"));
    }

    if (FirstOrSecondCompile(assembler) == FIRST_COMPILE) {
        assembler->cnt_current_label = 0;

        CHECK_AND_RETURN_ERRORS_ASM(CheckDoubleLabels(assembler));
    }

    CHECK_AND_RETURN_ERRORS_ASM(AsmVerify(assembler));

    return ASM_SUCCESS;
}

void InsertLabel(Assembler* assembler, char* string) {
    assert(string);

    assembler->about_labels[assembler->cnt_current_label].hash  = hash_djb2((char*)++string); // ++ because skip :
    assembler->about_labels[assembler->cnt_current_label].index = (type_t)assembler->byte_code_data.size;

    assembler->cnt_current_label++;
}

status_cmp FindCommand(Assembler* assembler, char* string, Asm_about_commands* current_command) {
    assert(assembler);
    assert(string);
    assert(current_command);

    unsigned long current_hash = hash_djb2((char*)string);

    Asm_about_commands* res = (Asm_about_commands*)bsearch(&current_hash, assembler->asm_about_commands, 
                                                           sizeof(assembler->asm_about_commands) / sizeof(assembler->asm_about_commands[0]), 
                                                           sizeof(Asm_about_commands), &bsearch_commands_comparator);

    if (res == NULL)
        return DIFFERENT;

    long int index = res - assembler->asm_about_commands;

    if (strcmp(assembler->asm_about_commands[index].name, string) == 0) {
        *current_command = assembler->asm_about_commands[index];
        return EQUAL;
    }

    return DIFFERENT;
}

void CheckPushPopCommands(Assembler* assembler, char* string, Asm_about_commands* current_command) {
    assert(assembler);
    assert(string);
    assert(current_command);

    if (current_command->hash == hash_djb2("PUSH")) {
        if (*string == '[' && *(string + 1) == 'R') { // [R_X]
            current_command->type_argument = RAM_REG_ARGUMENT;
            current_command->code          = CMD_PUSHMR;
        }
        else if (*string == '[') { // [0]
            current_command->type_argument = RAM_NUM_ARGUMENT;
            current_command->code          = CMD_PUSHMN;
        }
        else if (*string == 'R') {
            current_command->type_argument = REG_ARGUMENT;
            current_command->code          = CMD_PUSHR;
        }
    }

    if (current_command->hash == hash_djb2("POP")) {
        if (*string == '[' && *(string + 1) == 'R') { // [RAX]
            current_command->type_argument = RAM_REG_ARGUMENT;
            current_command->code          = CMD_POPMR;
        }
        else if (*string == '[') { // [0]
            current_command->type_argument = RAM_NUM_ARGUMENT;
            current_command->code          = CMD_POPMN;
        }
        else if (*string == 'R') { // RAX
            current_command->type_argument = REG_ARGUMENT;
            current_command->code          = CMD_POPR;
        }
    }
}

void FillCommand(Assembler* assembler, Asm_about_commands* current_command) {
    assert(assembler);
    assert(current_command);

    if (assembler->byte_code_data.data != NULL) {
        assembler->byte_code_data.data[assembler->byte_code_data.size] = current_command->code;
    }

    assembler->byte_code_data.size++;
}

assembler_status PassArgs(Assembler* assembler, Asm_about_commands* current_command, char* string) {
    if (current_command->type_argument == NUM_ARGUMENT) {
        if (GetFillArgNum(assembler, current_command, string) == ASM_SUCCESS)
            return ASM_SUCCESS;
    }

    if (current_command->type_argument == REG_ARGUMENT ||
        current_command->type_argument == RAM_REG_ARGUMENT ||
        current_command->type_argument == RAM_NUM_ARGUMENT) {
        if (GetFillArgReg(assembler, current_command, string, current_command->type_argument) == ASM_SUCCESS)
            return ASM_SUCCESS;
    }

    if (current_command->type_argument == LABEL_ARGUMENT) {
        if (GetFillArgJump(assembler, current_command, string) == ASM_SUCCESS)
            return ASM_SUCCESS;
    }

    return ASM_EXPECTS_ARG;
}

assembler_status GetFillArgNum(Assembler* assembler, Asm_about_commands* current_command, char* string) {
    assert(string);

    CHECK_AND_RETURN_ERRORS_ASM(AsmVerify(assembler));

    type_t number = 0;

    if (sscanf(string, TYPE_T_PRINTF_SPECIFIER, &number) == 1) {
        if (FirstOrSecondCompile(assembler) == SECOND_COMPILE) {
            assembler->byte_code_data.data[assembler->byte_code_data.size] = number;
            current_command->argument = number;
        }

        assembler->byte_code_data.size++;
    }
    else {
        return ASM_EXPECTS_NUMBER;
    }

    CHECK_AND_RETURN_ERRORS_ASM(AsmVerify(assembler));

    return ASM_SUCCESS;
}

assembler_status GetFillArgReg(Assembler* assembler, Asm_about_commands* current_command, char* string, int type_argument) {
    assert(string);

    CHECK_AND_RETURN_ERRORS_ASM(AsmVerify(assembler));

    type_t code = 0;
    if (!CheckRegister(string, type_argument, &code))
        return ASM_EXPECTS_REGISTER;

    if (FirstOrSecondCompile(assembler) == SECOND_COMPILE) {
        assembler->byte_code_data.data[assembler->byte_code_data.size] = code;
        current_command->argument = code;
    }

    assembler->byte_code_data.size++;

    CHECK_AND_RETURN_ERRORS_ASM(AsmVerify(assembler));

    return ASM_SUCCESS;
}

status_cmp CheckRegister(char* string, int type_argument, type_t* code) {
    assert(string);

    unsigned long current_hash = hash_djb2((char*)string);

    About_register* res = (About_register*)bsearch(&current_hash, about_register, sizeof(about_register) / sizeof(about_register[0]), 
                                                     sizeof(About_register), &bsearch_register_comparator);

    if (res == NULL && type_argument == RAM_NUM_ARGUMENT && string[0] == '[') {
        type_t number = 0;
        int read_bytes = 0;
        
        if (sscanf(++string, TYPE_T_PRINTF_SPECIFIER "%n", &number, &read_bytes) != 1)
            return DIFFERENT;
        string += read_bytes;
        
        if (string[0] == ']') {
            *code = number;
            return EQUAL;
        }   

        return DIFFERENT;
    } 
    else if (res == NULL)
        return DIFFERENT;

    long int index = res - about_register;

    if (strncmp(about_register[index].register_name, string, LEN_NAME_REGISTER) == 0) {
        if (type_argument == REG_ARGUMENT && strlen(string) == LEN_NAME_REGISTER) {
            *code = string[1] - 'A'; // R_X
            return EQUAL;
        }    

        if (type_argument == RAM_REG_ARGUMENT && strlen(string) == LEN_NAME_REGISTER + 2 // +2 because name has '[' at the begin and ']' at the end
                                                && string[0] == '['
                                                && string[LEN_NAME_REGISTER + 1] == ']') {
            *code = string[2] - 'A'; // [R_X]
            return EQUAL;
        }
    }

    return DIFFERENT;
}

assembler_status GetFillArgJump(Assembler* assembler, Asm_about_commands* current_command, char* string) {
    assert(string);

    if (GetFillArgNum(assembler, current_command, string) == ASM_SUCCESS) {
        return ASM_SUCCESS;
    }

    if (*string != ':') {
        return ASM_EXPECTS_JUMP_ARG;
    }

    if (FirstOrSecondCompile(assembler) == FIRST_COMPILE) {
        assembler->byte_code_data.size++;

        return ASM_SUCCESS;
    }

    unsigned long current_hash = hash_djb2((char*)++string); // ++ because skip :

    for (size_t i = 0; i < sizeof(assembler->about_labels) / sizeof(assembler->about_labels[0]); ++i) {
        if (current_hash != assembler->about_labels[i].hash)
            continue;

        if (FirstOrSecondCompile(assembler) == SECOND_COMPILE) {
            if (assembler->about_labels[i].index == -1)
                return ASM_NOT_FOUND_LABEL;

            assembler->byte_code_data.data[assembler->byte_code_data.size] = assembler->about_labels[i].index;
            current_command->argument = assembler->about_labels[i].index;
        }

        assembler->byte_code_data.size++;

        return ASM_SUCCESS;

    }

    return ASM_EXPECTS_JUMP_ARG;
}

void FillListingFile(char* pointer_on_command, Asm_about_commands* current_command, FILE* listing_file, int type_argument) {
    assert(pointer_on_command);
    assert(current_command);
    assert(listing_file);

    fprintf(listing_file, "%p ", pointer_on_command);
    fprintf(listing_file, "%-8s ", current_command->name);
    fprintf(listing_file, "%-5ld ", current_command->code);

    if (type_argument != NO_ARGUMENT) {
        fprintf(listing_file, TYPE_T_PRINTF_SPECIFIER, current_command->argument);
    }

    fprintf(listing_file, "\n");
}

assembler_status PrintfByteCode(Assembler* assembler) {
    CHECK_AND_RETURN_ERRORS_ASM(AsmVerify(assembler));

    fprintf(stderr, "Compile number %d:\n", FirstOrSecondCompile(assembler));

    if (FirstOrSecondCompile(assembler) == SECOND_COMPILE) {
        fprintf(stderr, "Byte code:\n");

        for (size_t i = 0; i < assembler->byte_code_data.size; ++i) {
            fprintf(stderr, TYPE_T_PRINTF_SPECIFIER " ", assembler->byte_code_data.data[i]);
        }

        fprintf(stderr, "\n");
    }

    fprintf(stderr, "Labels:\n");
    for (size_t i = 0; i < sizeof(assembler->about_labels) / sizeof(assembler->about_labels[0]); ++i) {
        fprintf(stderr, "hash %lu, index " TYPE_T_PRINTF_SPECIFIER "\n", assembler->about_labels[i].hash, assembler->about_labels[i].index);
    }
    fprintf(stderr, "\n\n");

    return ASM_SUCCESS;
}

assembler_status CheckDoubleLabels(Assembler* assembler) {
    qsort(assembler->about_labels, sizeof(assembler->about_labels) / sizeof(assembler->about_labels[0]), 
          sizeof(About_labels), &qsort_label_comparator);

    for (size_t i = 0; i < sizeof(assembler->about_labels) / sizeof(assembler->about_labels[0]) - 1; ++i) {
        if (assembler->about_labels[i].hash == assembler->about_labels[i + 1].hash &&
            assembler->about_labels[i].hash != 0) {
            CHECK_AND_RETURN_ERRORS_ASM(ASM_DOUBLE_LABEL);
        }
    }

    return ASM_SUCCESS;
}

assembler_status CreatByteCodeData(Assembler* assembler) {
    assert(assembler);

    assembler->byte_code_data.data = (type_t*)calloc(assembler->byte_code_data.size, sizeof(type_t));

    if (assembler->byte_code_data.data == NULL){
        CHECK_AND_RETURN_ERRORS_ASM(ASM_NOT_ENOUGH_MEMORY);
    }

    assembler->byte_code_data.size = 0;

    return ASM_SUCCESS;
}

assembler_status CreateExeFile(const Assembler* assembler, const char* name_byte_code_file) {
    assert(name_byte_code_file);
    assert(assembler->about_text.pointer_on_text);
    CHECK_AND_RETURN_ERRORS_ASM(AsmVerify(assembler));

    FILE* text = fopen(name_byte_code_file, "w");
    if (text == NULL) {
        CHECK_AND_RETURN_ERRORS_ASM(ASM_OPEN_ERROR,       perror("Error is"));
    }

    fprintf(text, "%zu\n", assembler->byte_code_data.size);

    for (size_t i = 0; i < assembler->byte_code_data.size; ++i) {
        fprintf(text, TYPE_T_PRINTF_SPECIFIER " ", assembler->byte_code_data.data[i]);
    }

    if (fclose(text) == EOF) {
        CHECK_AND_RETURN_ERRORS_ASM(ASM_CLOSE_ERROR,      perror("Error is"));
    }

    return ASM_SUCCESS;
}

assembler_status AsmDtor(Assembler* assembler) {
    assembler_status code_error = AsmVerify(assembler);

    free(assembler->byte_code_data.data);
    free(assembler->about_text.text);
    free(assembler->about_text.pointer_on_text);

    return code_error;
}
