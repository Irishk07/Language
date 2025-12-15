#include <assert.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#include "processor.h"

#include "check_errors.h"
#include "math.h"
#include "../proc_common.h"
#include "onegin.h"
#include "stack.h"
#include "variable_information.h"


processor_status ProcCtor(Processor* processor, const char* name_byte_code_file) {
    assert(processor);
    assert(name_byte_code_file);

    processor->about_text.text_name = name_byte_code_file;
    processor->programm_cnt = 0;
    processor->cnt_commands = 0;

    CHECK_AND_RETURN_ERRORS_STACK(STACK_CREATE(processor->stack, DEFAULT_START_CAPACITY));
    CHECK_AND_RETURN_ERRORS_STACK(STACK_CREATE(processor->return_stack, DEFAULT_START_CAPACITY));

    ProcInitAboutCommands(processor);

    memset(processor->registers, 0, CNT_REGISTERS * sizeof(processor->registers[0]));
    memset(processor->ram, 0, SIZE_RAM * sizeof(processor->ram[0]));

    CHECK_AND_RETURN_ERRORS_PROC(OneginReadFile(processor));

    CHECK_AND_RETURN_ERRORS_PROC(DivisionIntoCommands(processor));

    CHECK_AND_RETURN_ERRORS_PROC(ProcVerify(processor));

    return PROC_SUCCESS;
}

void ProcInitAboutCommands(Processor* processor) {
    assert(processor);

    for (size_t i = 0; i < SIZE_ABOUT_COMMANDS; ++i) {
        processor->proc_about_commands[about_commands[i].code] = {.name = about_commands[i].name,
                                                                  .function = about_commands[i].function};
    }
}

processor_status ProcVerify(const Processor* processor) {
    if (processor == NULL) {
        return PROC_NULL_POINTER_ON_STRUCT;
    }

    if (processor->about_text.pointer_on_text == NULL)          return PROC_NULL_POINTER_ON_DATA;

    if (processor->about_text.text_name == NULL)                return PROC_NULL_POINTER_ON_FILE;

    if (StackVerify(&processor->stack) != STACK_SUCCESS)        return STACK_ERROR;

    if (StackVerify(&processor->return_stack) != STACK_SUCCESS) return STACK_ERROR;

    return PROC_SUCCESS;
}

processor_status SPU(Processor* processor) {
    CHECK_AND_RETURN_ERRORS_PROC(ProcVerify(processor));

    type_t command = 0;

    while (processor->programm_cnt != processor->cnt_commands) {
        if (sscanf(processor->about_text.pointer_on_text[processor->programm_cnt], TYPE_T_PRINTF_SPECIFIER, &command) != 1)
            CHECK_AND_RETURN_ERRORS_PROC(PROC_WRONG_BYTE_CODE);

        if (command == CMD_HLT)
            break;

        processor->proc_about_commands[command].function(processor);

        processor->programm_cnt++;
    }

    if (processor->programm_cnt == processor->cnt_commands)
        CHECK_AND_RETURN_ERRORS_PROC(PROC_EXPECTS_HLT);

    CHECK_AND_RETURN_ERRORS_PROC(ProcVerify(processor));

    return PROC_SUCCESS;
}

void ProcDump(const Processor* processor, type_error_t code_error, int line, const char* function_name, const char* file_name) {
    fprintf(stderr, "Called at %s() %s:%d:\n", function_name, file_name, line);

    fprintf(stderr, "Count commands is: %zu\n", processor->cnt_commands);
    fprintf(stderr, "Now command is: %zu\n", processor->programm_cnt + 1);

    fprintf(stderr, "    registers[%d]:\n", CNT_REGISTERS);
    for (int i = 0; i < CNT_REGISTERS; ++i) {
        fprintf(stderr, "      [%d] = " TYPE_T_PRINTF_SPECIFIER " (R%cX)\n", i, processor->registers[i], i + 'A');
    }

    fprintf(stderr, "    ram[%d]:\n", SIZE_RAM);
    for (int i = 0; i < SIZE_RAM; ++i) {
        fprintf(stderr, "      [%d] = " "%d\n", i, processor->ram[i]);
    }

    fprintf(stderr, "\n\n");
    fprintf(stderr, "About stack:\n");
    StackDump(&processor->stack, code_error, DUMP_VAR_INFO);
}

processor_status ProcDtor(Processor* processor) {
    assert(processor);

    processor_status code_error = ProcVerify(processor);

    StackDtor(&processor->stack);
    StackDtor(&processor->return_stack);

    free(processor->about_text.text);
    free(processor->about_text.pointer_on_text);

    *processor = {};

    return code_error;
}


processor_status do_push(Processor* processor) {
    assert(processor);
    assert(processor->about_text.pointer_on_text);

    type_t num = 0;
    processor->programm_cnt++;

    if (sscanf(processor->about_text.pointer_on_text[processor->programm_cnt], TYPE_T_PRINTF_SPECIFIER, &num) != 1)
        return PROC_EXPECTS_ARG;

    CHECK_AND_RETURN_ERRORS_STACK(StackPush(&processor->stack, num));

    return PROC_SUCCESS;
}

processor_status do_pop(Processor* processor) {
    assert(processor);

    type_t deleted_value = 0;

    CHECK_AND_RETURN_ERRORS_STACK(StackPop(&processor->stack, &deleted_value));

    return PROC_SUCCESS;
}

processor_status do_add(Processor* processor) {
    assert(processor);

    type_t first_num  = 0;
    type_t second_num = 0;

    CHECK_AND_RETURN_ERRORS_STACK(StackPop(&processor->stack, &first_num));
    CHECK_AND_RETURN_ERRORS_STACK(StackPop(&processor->stack, &second_num));
    CHECK_AND_RETURN_ERRORS_STACK(StackPush(&processor->stack, second_num + first_num));

    return PROC_SUCCESS;
}

processor_status do_sub(Processor* processor) {
    assert(processor);

    type_t first_num  = 0;
    type_t second_num = 0;

    CHECK_AND_RETURN_ERRORS_STACK(StackPop(&processor->stack, &first_num));
    CHECK_AND_RETURN_ERRORS_STACK(StackPop(&processor->stack, &second_num));
    CHECK_AND_RETURN_ERRORS_STACK(StackPush(&processor->stack, second_num - first_num));

    return PROC_SUCCESS;
}

processor_status do_div(Processor* processor) {
    assert(processor);

    type_t first_num  = 0;
    type_t second_num = 0;

    CHECK_AND_RETURN_ERRORS_STACK(StackPop(&processor->stack, &first_num));
    CHECK_AND_RETURN_ERRORS_STACK(StackPop(&processor->stack, &second_num));

    if (first_num != 0) {
        CHECK_AND_RETURN_ERRORS_STACK(StackPush(&processor->stack, second_num / first_num));
    }
    else {
        return PROC_DIVISION_BY_ZERO;
    }

    return PROC_SUCCESS;
}

processor_status do_mul(Processor* processor) {
    assert(processor);

    type_t first_num  = 0;
    type_t second_num = 0;

    CHECK_AND_RETURN_ERRORS_STACK(StackPop(&processor->stack, &first_num));
    CHECK_AND_RETURN_ERRORS_STACK(StackPop(&processor->stack, &second_num));
    CHECK_AND_RETURN_ERRORS_STACK(StackPush(&processor->stack, second_num * first_num));

    return PROC_SUCCESS;
}

processor_status do_sqrt(Processor* processor) {
    assert(processor);

    type_t num = 0;

    CHECK_AND_RETURN_ERRORS_STACK(StackPop(&processor->stack, &num));

    if (num >= 0) {
        CHECK_AND_RETURN_ERRORS_STACK(StackPush(&processor->stack, (type_t)sqrt(num)));
    }
    else {
        return PROC_SQRT_NEGATIVE_NUM;
    }

    return PROC_SUCCESS;
}

processor_status do_pow(Processor* processor) {
    assert(processor);

    type_t first_num  = 0;
    type_t second_num = 0;

    CHECK_AND_RETURN_ERRORS_STACK(StackPop(&processor->stack, &first_num));
    CHECK_AND_RETURN_ERRORS_STACK(StackPop(&processor->stack, &second_num));
    CHECK_AND_RETURN_ERRORS_STACK(StackPush(&processor->stack, (type_t)pow(second_num, first_num)));

    return PROC_SUCCESS;
}

processor_status do_in(Processor* processor) {
    assert(processor);

    type_t num  = 0;

    printf("Enter tne number please:\n");

    if (scanf(TYPE_T_PRINTF_SPECIFIER, &num) != 1)
        return PROC_EXPECTS_ARG;

    CHECK_AND_RETURN_ERRORS_STACK(StackPush(&processor->stack, num));

    return PROC_SUCCESS;
}

processor_status do_popr(Processor* processor) {
    assert(processor);
    assert(processor->about_text.pointer_on_text);

    type_t num = 0;
    type_t code_reg  = 0;

    CHECK_AND_RETURN_ERRORS_STACK(StackPop(&processor->stack, &num));
    processor->programm_cnt++;

    if (sscanf(processor->about_text.pointer_on_text[processor->programm_cnt], TYPE_T_PRINTF_SPECIFIER, &code_reg) != 1)
        return PROC_EXPECTS_ARG;

    if (code_reg < 0 || code_reg >= CNT_REGISTERS)
        CHECK_AND_RETURN_ERRORS_PROC(PROC_INVALID_REGISTER);

    processor->registers[code_reg] = num;

    return PROC_SUCCESS;
}

processor_status do_pushr(Processor* processor) {
    assert(processor);
    assert(processor->about_text.pointer_on_text);

    type_t code_reg  = 0;
    processor->programm_cnt++;

    if (sscanf(processor->about_text.pointer_on_text[processor->programm_cnt], TYPE_T_PRINTF_SPECIFIER, &code_reg) != 1)
        return PROC_EXPECTS_ARG;

    if (code_reg < 0 || code_reg >= CNT_REGISTERS)
        CHECK_AND_RETURN_ERRORS_PROC(PROC_INVALID_REGISTER);

    CHECK_AND_RETURN_ERRORS_STACK(StackPush(&processor->stack, processor->registers[code_reg]));

    return PROC_SUCCESS;
}

processor_status do_out(Processor* processor) {
    assert(processor);

    type_t num = 0;

    CHECK_AND_RETURN_ERRORS_STACK(StackPop(&processor->stack, &num));
    fprintf(stderr, "Answer " TYPE_T_PRINTF_SPECIFIER "\n", num);

    return PROC_SUCCESS;
}

processor_status do_jmp(Processor* processor) {
    assert(processor);
    assert(processor->about_text.pointer_on_text);

    type_t num = 0;
    processor->programm_cnt++;

    if (sscanf(processor->about_text.pointer_on_text[processor->programm_cnt], TYPE_T_PRINTF_SPECIFIER, &num) != 1)
        return PROC_EXPECTS_ARG;

    processor->programm_cnt = (size_t)(num - 1);

    // getchar();

    return PROC_SUCCESS;
}

#define DO_JUMP_CONDITION(name, sign)                                       \
                                                                            \
processor_status do_ ## name(Processor* processor) {                        \
    type_t first_num  = 0;                                                  \
    type_t second_num = 0;                                                  \
                                                                            \
    CHECK_AND_RETURN_ERRORS_STACK(StackPop(&processor->stack, &first_num)); \
    CHECK_AND_RETURN_ERRORS_STACK(StackPop(&processor->stack, &second_num));\
                                                                            \
    if (second_num sign first_num) {                                        \
        CHECK_AND_RETURN_ERRORS_PROC(do_jmp(processor));                    \
    }                                                                       \
    else {                                                                  \
        processor->programm_cnt++;                                          \
    }                                                                       \
                                                                            \
    return PROC_SUCCESS;                                                    \
}

DO_JUMP_CONDITION(ja,  >)
DO_JUMP_CONDITION(jae, >=)
DO_JUMP_CONDITION(jb,  <)
DO_JUMP_CONDITION(jbe, <=)
DO_JUMP_CONDITION(je,  ==)
DO_JUMP_CONDITION(jne, !=)

#undef DO_JUMP_CONDITION

processor_status do_call(Processor* processor) {
    assert(processor);

    CHECK_AND_RETURN_ERRORS_STACK(StackPush(&processor->return_stack, (type_t)processor->programm_cnt + 2));

    CHECK_AND_RETURN_ERRORS_PROC(do_jmp(processor));

    return PROC_SUCCESS;
}

processor_status do_ret(Processor* processor) {
    assert(processor);

    type_t last_value = 0;
    CHECK_AND_RETURN_ERRORS_STACK(StackPop(&processor->return_stack, &last_value));

    processor->programm_cnt = (size_t)last_value - 1; // -1 because after this function in spu processor->programm_cnt++

    return PROC_SUCCESS;
}

processor_status do_pushmr(Processor* processor) {
    assert(processor);

    processor->programm_cnt++;

    type_t reg = 0;
    if (sscanf(processor->about_text.pointer_on_text[processor->programm_cnt], TYPE_T_PRINTF_SPECIFIER, &reg) != 1)
        return PROC_EXPECTS_ARG;

    CHECK_AND_RETURN_ERRORS_STACK(StackPush(&processor->stack, processor->ram[processor->registers[reg]]));

    return PROC_SUCCESS;
}

processor_status do_popmr(Processor* processor) {
    assert(processor);

    type_t num = 0;
    CHECK_AND_RETURN_ERRORS_STACK(StackPop(&processor->stack, &num));

    processor->programm_cnt++;

    type_t reg = 0;
    if (sscanf(processor->about_text.pointer_on_text[processor->programm_cnt], TYPE_T_PRINTF_SPECIFIER, &reg) != 1)
        return PROC_EXPECTS_ARG;

    processor->ram[processor->registers[reg]] = (int)num;

    // draw_ram(processor);

    return PROC_SUCCESS;
}

processor_status do_pushmn(Processor* processor) {
    assert(processor);

    processor->programm_cnt++;

    type_t reg = 0;
    if (sscanf(processor->about_text.pointer_on_text[processor->programm_cnt], TYPE_T_PRINTF_SPECIFIER, &reg) != 1)
        return PROC_EXPECTS_ARG;

    CHECK_AND_RETURN_ERRORS_STACK(StackPush(&processor->stack, processor->ram[reg]));

    return PROC_SUCCESS;
}

processor_status do_popmn(Processor* processor) {
    assert(processor);

    type_t num = 0;
    CHECK_AND_RETURN_ERRORS_STACK(StackPop(&processor->stack, &num));

    processor->programm_cnt++;

    type_t reg = 0;
    if (sscanf(processor->about_text.pointer_on_text[processor->programm_cnt], TYPE_T_PRINTF_SPECIFIER, &reg) != 1)
        return PROC_EXPECTS_ARG;

    processor->ram[reg] = (int)num;

    // draw_ram(processor);

    return PROC_SUCCESS;
}

processor_status do_ln(Processor* processor) {
    assert(processor);

    type_t num = 0;

    CHECK_AND_RETURN_ERRORS_STACK(StackPop(&processor->stack, &num));

    if (num > 0) {
        CHECK_AND_RETURN_ERRORS_STACK(StackPush(&processor->stack, (type_t)log(num)));
    }
    else
        return PROC_LN_NEGATIVE_NUM;

    return PROC_SUCCESS;
}

processor_status do_log(Processor* processor) {
    assert(processor);

    type_t first_num  = 0;
    type_t second_num = 0;

    CHECK_AND_RETURN_ERRORS_STACK(StackPop(&processor->stack, &first_num));
    CHECK_AND_RETURN_ERRORS_STACK(StackPop(&processor->stack, &second_num));

    if (first_num > 0 && first_num != 1 && second_num > 0) {
        CHECK_AND_RETURN_ERRORS_STACK(StackPush(&processor->stack, (type_t)(log(second_num) / log(first_num))));
    }
    else
        return PROC_LOG_NEGATIVE_NUM;

    return PROC_SUCCESS;
}

processor_status do_sin(Processor* processor) {
    assert(processor);

    type_t num = 0;

    CHECK_AND_RETURN_ERRORS_STACK(StackPop(&processor->stack, &num));
    CHECK_AND_RETURN_ERRORS_STACK(StackPush(&processor->stack, (type_t)sin(num)));

    return PROC_SUCCESS;
}

processor_status do_cos(Processor* processor) {
    assert(processor);

    type_t num = 0;

    CHECK_AND_RETURN_ERRORS_STACK(StackPop(&processor->stack, &num));
    CHECK_AND_RETURN_ERRORS_STACK(StackPush(&processor->stack, (type_t)cos(num)));

    return PROC_SUCCESS;
}

processor_status do_tg(Processor* processor) {
    assert(processor);

    type_t num = 0;

    CHECK_AND_RETURN_ERRORS_STACK(StackPop(&processor->stack, &num));
    CHECK_AND_RETURN_ERRORS_STACK(StackPush(&processor->stack, (type_t)tan(num)));

    return PROC_SUCCESS;
}

processor_status do_ctg(Processor* processor) {
    assert(processor);

    type_t num = 0;
    CHECK_AND_RETURN_ERRORS_STACK(StackPop(&processor->stack, &num));
    double tg = tan(num);

    if (fabs(tg) >= 1e-10) {
        CHECK_AND_RETURN_ERRORS_STACK(StackPush(&processor->stack, (type_t)(1.0 / tg)));
    }
    else
        return PROC_CTG_NEGATIVE_NUM;

    return PROC_SUCCESS;
}

processor_status do_arcsin(Processor* processor) {
    assert(processor);

    type_t num = 0;

    CHECK_AND_RETURN_ERRORS_STACK(StackPop(&processor->stack, &num));
    if (num >= -1 && num <= 1) {
        CHECK_AND_RETURN_ERRORS_STACK(StackPush(&processor->stack, (type_t)asin(num)));
    }
    else
        return PROC_ARCSIN_NEGATIVE_NUM;

    return PROC_SUCCESS;
}

processor_status do_arccos(Processor* processor) {
    assert(processor);

    type_t num = 0;

    CHECK_AND_RETURN_ERRORS_STACK(StackPop(&processor->stack, &num));
    if (num >= -1 && num <= 1) {
        CHECK_AND_RETURN_ERRORS_STACK(StackPush(&processor->stack, (type_t)acos(num)));
    }
    else
        return PROC_ARCCOS_NEGATIVE_NUM;

    return PROC_SUCCESS;
}

processor_status do_arctg(Processor* processor) {
    assert(processor);

    type_t num = 0;

    CHECK_AND_RETURN_ERRORS_STACK(StackPop(&processor->stack, &num));
    CHECK_AND_RETURN_ERRORS_STACK(StackPush(&processor->stack, (type_t)atan(num)));

    return PROC_SUCCESS;
}

processor_status do_arcctg(Processor* processor) {
    assert(processor);

    type_t num = 0;

    CHECK_AND_RETURN_ERRORS_STACK(StackPop(&processor->stack, &num));
    CHECK_AND_RETURN_ERRORS_STACK(StackPush(&processor->stack, (type_t)(M_PI_2 - atan(num))));

    return PROC_SUCCESS;
}

processor_status do_sh(Processor* processor) {
    assert(processor);

    type_t num = 0;

    CHECK_AND_RETURN_ERRORS_STACK(StackPop(&processor->stack, &num));
    CHECK_AND_RETURN_ERRORS_STACK(StackPush(&processor->stack, (type_t)sinh(num)));

    return PROC_SUCCESS;
}

processor_status do_ch(Processor* processor) {
    assert(processor);

    type_t num = 0;

    CHECK_AND_RETURN_ERRORS_STACK(StackPop(&processor->stack, &num));
    CHECK_AND_RETURN_ERRORS_STACK(StackPush(&processor->stack, (type_t)cosh(num)));

    return PROC_SUCCESS;
}

processor_status do_th(Processor* processor) {
    assert(processor);

    type_t num = 0;

    CHECK_AND_RETURN_ERRORS_STACK(StackPop(&processor->stack, &num));
    CHECK_AND_RETURN_ERRORS_STACK(StackPush(&processor->stack, (type_t)tanh(num)));

    return PROC_SUCCESS;
}

processor_status do_cth(Processor* processor) {
    assert(processor);

    type_t num = 0;

    CHECK_AND_RETURN_ERRORS_STACK(StackPop(&processor->stack, &num));
    if (fabs(sinh(num)) >= 1e-10) {
        CHECK_AND_RETURN_ERRORS_STACK(StackPush(&processor->stack, (type_t)(cosh(num) / sinh(num))));
    }
    else
        return PROC_CTH_NEGATIVE_NUM;

    return PROC_SUCCESS;
}

void draw_ram(Processor* processor) {
    assert(processor);

    for (int i = 0; i < SIZE_RAM; ++i) {
        if (i % 10 == 0)
            printf ("\n");

        if (processor->ram[i] == 1)
            printf("%c ", '*');
        else
            printf("%c ", '.');
    }

    printf("\n");
}


/*

why break?

while (bytes != 0) {
    switch(bytes % 8) {
        case 0:
            memcpy(8); bytes -= 8;
            break;
        case 7:
        case 6:
        case 5:
        case 4:
            memcpy(4); bytes -= 4;
            break;
        case 3:
        case 2:
            memcpy(2); bytes -= 2;
            break;
        case 1:
            memcpy(1); bytes -= 1;
            break;
        default:
            assert(0);
    }
}

*/
