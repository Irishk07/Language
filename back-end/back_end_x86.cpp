#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "back_end_x86.h"

#include "../array.h"
#include "back_end_bin.h"
#include "../common.h"
#include "common_back.h"
#include "onegin.h"
#include "../tree.h"


// objdump, readelf



Tree_status BackEndX86(Language* language, const char* name_asm_file, const char* name_bin_file) {
    assert(language);
    assert(name_asm_file);  

    FILE* asm_file = fopen(name_asm_file, "w");
    if (asm_file == NULL)
        TREE_CHECK_AND_RETURN_ERRORS(OPEN_ERROR);
    FILE* bin_file = fopen(name_bin_file, "wb");
    if (bin_file == NULL)
        TREE_CHECK_AND_RETURN_ERRORS(OPEN_ERROR);

    if (is_second_pass) GenerateElf(bin_file);

    fprintf(asm_file, "default rel\n\n");

    SECTION_DATA_

    SECTION_BSS_

    fprintf(asm_file, "section .text\n\n");
    fprintf(asm_file, "global main\n\n");
    FUNC_("main");

    CreateAsmFileX86(language, language->tree.root, asm_file, bin_file);

    PrintFuncMyPrintf(language, asm_file, bin_file);
    PrintFuncMyScanf(language, asm_file, bin_file);

    fprintf(asm_file, "section .note.GNU-stack noalloc noexec nowrite progbits\n");

    if (is_second_pass) WriteJumpTable(bin_file);
    if (is_second_pass) FinalizeElfSize(bin_file);

    if (fclose(asm_file) == EOF)
        TREE_CHECK_AND_RETURN_ERRORS(CLOSE_ERROR);
    if (fclose(bin_file) == EOF)
        TREE_CHECK_AND_RETURN_ERRORS(CLOSE_ERROR);

    return SUCCESS;
}

void CreateAsmFileX86(Language* language, Tree_node* tree_node, FILE* asm_file, FILE* bin_file) {
    assert(language);
    assert(asm_file);

    if (tree_node == NULL)
        return;

    if (tree_node->type == NUMBER) {
        PUSH_NUM_(tree_node->value.number);
    }    

    if (tree_node->type == VARIABLE) {
        size_t size = language->array_with_variables.size;
        for (size_t i = 0; i < language->array_with_variables.size; ++i) {
            if (strcmp(NameOfVariable(tree_node), NameOfVariableFromIndex(&language->array_with_variables, i)) == 0) {
                MOV_R1_R2_(R12, RBP);
                SUB_R1_R2_(R12, RBX);
                ADD_R_NUM_(R12, (int)(size - i) * 8);
                PUSH_M_(R12);
            }
        }   
    }            

    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wswitch-enum"
    if (tree_node->type == OPERATOR) {
        switch(tree_node->value.operators) {
            case OPERATOR_ADD:    PrintOperationsX86(language, tree_node, asm_file, bin_file, "add");      break;
            case OPERATOR_SUB:    PrintOperationsX86(language, tree_node, asm_file, bin_file, "sub");      break;
            case OPERATOR_MUL:    PrintOperationsX86(language, tree_node, asm_file, bin_file, "imul");     break;    
            case OPERATOR_DIV:    PrintOperationDivX86(language, tree_node, asm_file, bin_file);           break;   
            case OPERATOR_SQRT:   PrintOperationSqrtX86(language, tree_node, asm_file, bin_file);          break;  

            case OPERATOR_ASSIGNMENT: PrintAssignmentX86(language, tree_node, asm_file, bin_file);         break;
            case OPERATOR_CHANGE:     PrintChangeX86(language, tree_node, asm_file, bin_file);             break;

            case OPERATOR_IF:         PrintIfX86(language, tree_node, asm_file, bin_file);                 break;
            case OPERATOR_ELSE:                                                                            break;
            case OPERATOR_WHILE:      PrintWhileX86(language, tree_node, asm_file, bin_file);              break;

            case OPERATOR_COMMON:
            case OPERATOR_PARAM:  CreateAsmFileX86(language, tree_node->left_node, asm_file, bin_file);
                                  CreateAsmFileX86(language, tree_node->right_node, asm_file, bin_file);   break;

            case OPERATOR_INPUT:  PrintInX86(language, tree_node, asm_file, bin_file);                     break;
            case OPERATOR_PRINT:  PrintOutX86(language, tree_node, asm_file, bin_file);                    break;

            case OPERATOR_MAIN_FUNCTION: PrintMainFunctionX86(language, tree_node, asm_file, bin_file);    break;
            case OPERATOR_DEF_FUNCTION:  PrintDefFunctionX86(language, tree_node, asm_file, bin_file);     break;
            case OPERATOR_CALL_FUNCTION: PrintCallFunctionX86(language, tree_node, asm_file, bin_file);    break;
            case OPERATOR_RETURN:        PrintReturnFunctionX86(language, tree_node, asm_file, bin_file);  break;

            case OPERATOR_ABOVE:
            case OPERATOR_BEFORE:
            case OPERATOR_EQUAL: 
            case OPERATOR_OPEN_BRACKET:  
            case OPERATOR_CLOSE_BRACKET: 
            case OPERATOR_OPEN_FIGURE:   
            case OPERATOR_CLOSE_FIGURE:  
            case OPERATOR_FINISH_SYMBOL:
            case OPERATOR_MATCH:
            case WRONG_OPERATOR:
            default: break; 
        }
    }
    #pragma GCC diagnostic pop
}

void PrintOperationsX86(Language* language, Tree_node* tree_node, FILE* asm_file, FILE* bin_file, const char* operation) {
    assert(language);
    assert(asm_file);
    assert(tree_node);
    assert(operation);

    fprintf(asm_file, ";/ begin operation %s:\n", operation);

    CreateAsmFileX86(language, tree_node->left_node, asm_file, bin_file);
    CreateAsmFileX86(language, tree_node->right_node, asm_file, bin_file);

    POP_R_(R13);
    POP_R_(R12);
    if (strcmp("add", operation) == 0) {
        ADD_R1_R2_(R12, R13);
    }
    else if (strcmp("sub", operation) == 0) {
        SUB_R1_R2_(R12, R13);
    }
    else {
        IMUL_R1_R2_(R12, R13);
    }    
    PUSH_R_(R12);

    fprintf(asm_file, ";/ end operation %s\n\n", operation);
}

void PrintOperationDivX86(Language* language, Tree_node* tree_node, FILE* asm_file, FILE* bin_file) {
    assert(language);
    assert(asm_file);
    assert(tree_node);

    PUSH_R_(RAX);
    PUSH_R_(RDX);
    PUSH_R_(RBX);

    fprintf(asm_file, ";/ begin operation idiv:\n");

    CreateAsmFileX86(language, tree_node->left_node, asm_file, bin_file);
    CreateAsmFileX86(language, tree_node->right_node, asm_file, bin_file);

    POP_R_(RBX);
    POP_R_(RAX);
    CQO_;
    IDIV_R_(RBX);
    MOV_R1_R2_(R12, RAX);
    POP_R_(RBX);
    POP_R_(RDX);
    POP_R_(RAX);
    PUSH_R_(R12);

    fprintf(asm_file, ";/ end operation idiv\n\n");
}

void PrintOperationSqrtX86(Language* language, Tree_node* tree_node, FILE* asm_file, FILE* bin_file) {
    assert(language);
    assert(asm_file);
    assert(tree_node);

    fprintf(asm_file, ";/ begin operation sqrt:\n");

    CreateAsmFileX86(language, tree_node->left_node, asm_file, bin_file);

    POP_R_(R12);
    CVTSI2SD_(XMM0, R12);
    SQRTSD_(XMM1, XMM0);
    CVTTSD2SI_(R12, XMM1);
    PUSH_R_(R12);

    fprintf(asm_file, ";/ end operation sqrt\n\n");
}

void PrintAssignmentX86(Language* language, Tree_node* tree_node, FILE* asm_file, FILE* bin_file) {
    assert(language);
    assert(asm_file);
    assert(tree_node);

    fprintf(asm_file, ";/ begin assignment:\n");

    CreateAsmFileX86(language, tree_node->right_node, asm_file, bin_file);

    Tree_node* name_node = tree_node->left_node;

    About_variable about_variable = {.name = NameOfVariable(name_node), .value = (int)language->array_with_variables.size};
    
    POP_R_(R12);
    SUB_R_NUM_(RSP, 8);
    MOV_R1_R2_(RAX, RBP);
    SUB_R1_R2_(RAX, RBX);
    MOV_MR_R_(RAX, R12);
    
    ArrayPush(&language->array_with_variables, &about_variable);

    ADD_R_NUM_(RBX, 8);

    fprintf(asm_file, ";/ end assignment\n\n");
}

void PrintChangeX86(Language* language, Tree_node* tree_node, FILE* asm_file, FILE* bin_file) {
    assert(language);
    assert(asm_file);
    assert(tree_node);

    fprintf(asm_file, ";/ begin change:\n");

    CreateAsmFileX86(language, tree_node->right_node, asm_file, bin_file);

    size_t size = language->array_with_variables.size;
    for (size_t i = 0; i < language->array_with_variables.size; ++i) {
        if (strcmp(NameOfVariable(tree_node->left_node), NameOfVariableFromIndex(&language->array_with_variables, i)) == 0) {
            MOV_R1_R2_(R12, RBP);
            SUB_R1_R2_(R12, RBX);
            ADD_R_NUM_(R12, (int)(size - i) * 8);
            POP_M_(R12);
        }
    }         

    fprintf(asm_file, ";/ end change\n\n");
}

void PrintIfX86(Language* language, Tree_node* tree_node, FILE* asm_file, FILE* bin_file) {
    assert(language);
    assert(asm_file);
    assert(tree_node);

    size_t if_id = language->cnt_labels.cnt_if++;

    if (is_second_pass) fprintf(asm_file, ";/ begin if_%zu:\n", if_id);

    char label_end_if_name[128] = {};
    snprintf(label_end_if_name, 128, ".end_if_%zu", if_id);

    Tree_node* condition = tree_node->left_node;

    if (condition->type == OPERATOR && (condition->value.operators == OPERATOR_ABOVE ||
                                        condition->value.operators == OPERATOR_BEFORE ||
                                        condition->value.operators == OPERATOR_EQUAL)) {
        Type_operators type_operator = condition->value.operators;

        CreateAsmFileX86(language, condition->left_node, asm_file, bin_file);
        CreateAsmFileX86(language, condition->right_node, asm_file, bin_file);

        POP_R_(R13);
        POP_R_(R12);
        CMP_R1_R2_(R12, R13);

        char jump_label[128] = {};
        if (tree_node->right_node->type == OPERATOR && tree_node->right_node->value.operators == OPERATOR_ELSE)
            snprintf(jump_label, 128, ".else_%zu", language->cnt_labels.cnt_else);
        else
            strcpy(jump_label, label_end_if_name);

        uint64_t target_ip = GetLabelAddress(jump_label); 

        if (type_operator == OPERATOR_ABOVE)
            JLE_(jump_label, target_ip)
        else if (type_operator == OPERATOR_BEFORE)
            JGE_(jump_label, target_ip)
        if (type_operator == OPERATOR_EQUAL)
            JNE_(jump_label, target_ip)
    }
    else {
        CreateAsmFileX86(language, condition, asm_file, bin_file);
        PUSH_NUM_(0);

        char jump_label[128] = {};
        if (tree_node->right_node->type == OPERATOR && tree_node->right_node->value.operators == OPERATOR_ELSE)
            sprintf(jump_label, ".else_%zu", language->cnt_labels.cnt_else);
        else
            strcpy(jump_label, label_end_if_name);

        uint64_t target_ip = GetLabelAddress(label_end_if_name);
        JE_(jump_label, target_ip)
    }

    if (tree_node->right_node->type != OPERATOR || tree_node->right_node->value.operators != OPERATOR_ELSE)
        CreateAsmFileX86(language, tree_node->right_node, asm_file, bin_file);
    else
        PrintElseX86(language, tree_node->right_node, asm_file, bin_file, if_id);

    LABEL_(label_end_if_name)

    if (is_second_pass) fprintf(asm_file, ";/ end if_%zu\n\n", if_id);
}

void PrintElseX86(Language* language, Tree_node* tree_node, FILE* asm_file, FILE* bin_file, size_t if_id) {
    size_t current_else_id = language->cnt_labels.cnt_else++;

    char label_else_name[128] = {};
    snprintf(label_else_name, 128, ".else_%zu", current_else_id);
    char label_end_if_name[128] = {};
    snprintf(label_end_if_name, 128, ".end_if_%zu", if_id);

    CreateAsmFileX86(language, tree_node->left_node, asm_file, bin_file); 

    uint64_t target_end = GetLabelAddress(label_end_if_name);
    JMP_(label_end_if_name, target_end);

    LABEL_(label_else_name);

    CreateAsmFileX86(language, tree_node->right_node, asm_file, bin_file); 

    if (is_second_pass) fprintf(asm_file, ";/ end else_%zu\n\n", language->cnt_labels.cnt_else);
}

void PrintWhileX86(Language* language, Tree_node* tree_node, FILE* asm_file, FILE* bin_file) {
    assert(language);
    assert(asm_file);
    assert(tree_node);

    if (is_second_pass) fprintf(asm_file, ";/ begin while_%zu:\n", language->cnt_labels.cnt_while);

    char label_begin_while_name[128] = {};
    snprintf(label_begin_while_name, 128, ".begin_while_%zu", language->cnt_labels.cnt_while);
    char label_end_while_name[128] = {};
    snprintf(label_end_while_name, 128, ".end_while_%zu", language->cnt_labels.cnt_while);

    size_t cnt_while = language->cnt_labels.cnt_while;
    if (is_second_pass) fprintf(asm_file, "begin_while_%zu:\n", cnt_while);
    language->cnt_labels.cnt_while++;

    Tree_node* condition = tree_node->left_node;

    if (condition->type == OPERATOR && (condition->value.operators == OPERATOR_ABOVE ||
                                        condition->value.operators == OPERATOR_BEFORE ||
                                        condition->value.operators == OPERATOR_EQUAL)) {
        Type_operators type_operator = condition->value.operators;

        CreateAsmFileX86(language, condition->left_node, asm_file, bin_file);
        CreateAsmFileX86(language, condition->right_node, asm_file, bin_file);

        POP_R_(R13);
        POP_R_(R12);
        CMP_R1_R2_(R12, R13);

        uint64_t target_end = GetLabelAddress(label_end_while_name);
        if (type_operator == OPERATOR_ABOVE)
            JLE_(label_end_while_name, target_end)
        else if (type_operator == OPERATOR_BEFORE)
            JGE_(label_end_while_name, target_end)
        if (type_operator == OPERATOR_EQUAL)
            JNE_(label_end_while_name, target_end)
    }
    else {
        CreateAsmFileX86(language, condition, asm_file, bin_file);
        PUSH_NUM_(0);

        uint64_t target_end = GetLabelAddress(label_end_while_name);
        JE_(label_end_while_name, target_end)
    }

    CreateAsmFileX86(language, tree_node->right_node, asm_file, bin_file);
    uint64_t target_begin = GetLabelAddress(label_begin_while_name);
    JMP_(label_begin_while_name, target_begin);

    LABEL_(label_end_while_name);

    if (is_second_pass)fprintf(asm_file, ";/ end while_%zu\n\n", cnt_while);
}

void PrintInX86(Language* language, Tree_node* tree_node, FILE* asm_file, FILE* bin_file) {
    assert(language);
    assert(tree_node);
    assert(asm_file);

    fprintf(asm_file, ";/ begin in:\n");

    CreateAsmFileX86(language, tree_node->left_node, asm_file, bin_file);

    SUB_R_NUM_(RSP, 8);
    MOV_R1_R2_(R12, RSP);
    AND_R_NUM_(RSP, -16);
    MOV_FORMAT_IN_(RDI);
    MOV_R1_R2_(RSI, RBP);
    SUB_R1_R2_(RSI, RBX);
    XOR_R1_R2_(RAX, RAX);
    CALL_("my_scanf");
    MOV_R1_R2_(RSP, R12);

    fprintf(asm_file, ";/ end in\n\n");
}

void PrintOutX86(Language* language, Tree_node* tree_node, FILE* asm_file, FILE* bin_file) {
    assert(language);
    assert(tree_node);
    assert(asm_file);

    fprintf(asm_file, ";/ begin out:\n");

    CreateAsmFileX86(language, tree_node->left_node, asm_file, bin_file);

    POP_R_(RSI);
    MOV_R1_R2_(R12, RSP);
    AND_R_NUM_(RSP, -16);
    MOV_FORMAT_OUT_(RDI);
    MOV_R_NUM_(RAX, 0);
    CALL_("my_printf");
    MOV_R1_R2_(RSP, R12);

    fprintf(asm_file, ";/ end out\n\n");
}

void PrintMainFunctionX86(Language* language, Tree_node* tree_node, FILE* asm_file, FILE* bin_file) {
    assert(language);
    assert(tree_node);
    assert(asm_file);

    fprintf(asm_file, ";/ begin main:\n");

    language->array_with_variables = {};
    ArrayCtor(&(language->array_with_variables), sizeof(About_variable), DEFAULT_START_CAPACITY);

    PUSH_R_(RBP);
    MOV_R1_R2_(RBP, RSP);
    MOV_R_NUM_(RBX, 8);

    CreateAsmFileX86(language, tree_node->right_node, asm_file, bin_file);

    free(language->array_with_variables.data);

    MOV_R1_R2_(RSP, RBP);
    POP_R_(RBP);
    MOV_R_NUM_(RAX, 60);
    XOR_R1_R2_(RDI, RDI);
    SYSCALL_;

    fprintf(asm_file, ";/ end main\n\n");
}

void PrintDefFunctionX86(Language* language, Tree_node* tree_node, FILE* asm_file, FILE* bin_file) {
    assert(language);
    assert(tree_node);
    assert(asm_file);

    fprintf(asm_file, ";/ begin def function:\n");

    Tree_node* common_node = tree_node->left_node;
    Tree_node* name_node = common_node->left_node;

    language->array_with_variables = {};
    ArrayCtor(&(language->array_with_variables), sizeof(About_variable), DEFAULT_START_CAPACITY);

    FUNC_(NameOfVariable(name_node));

    int cnt_params = 0;
    common_node = common_node->right_node;
    while (common_node != NULL 
           && common_node->type == OPERATOR && common_node->value.operators == OPERATOR_PARAM) {
        About_variable about_variable = {.name = NameOfVariable(common_node->left_node), .value = cnt_params};
        ArrayPush(&language->array_with_variables, &about_variable);

        common_node = common_node->right_node;
        cnt_params++;
    }
    if (common_node != NULL) {
        About_variable about_variable = {.name = NameOfVariable(common_node), .value = cnt_params};
        ArrayPush(&language->array_with_variables, &about_variable);
        cnt_params++;
    }

    PUSH_R_(R14);
    PUSH_R_(RBP);
    MOV_R1_R2_(RBP, RSP);
    PUSH_R_(RBX);
    MOV_R_NUM_(RBX, 16);

    size_t size = language->array_with_variables.size;
    SUB_R_NUM_(RSP, (int)size * 8);
    for (int i = (int)size - 1; i >= 0; --i) {
        MOV_R1_R2_(R12, RBP);
        ADD_R_NUM_(R12,  ((int)size - i + 2) * 8);
        MOV_R_MR_(R12, R12);
        MOV_R1_R2_(R13, RBP);
        SUB_R1_R2_(R13, RBX);
        SUB_R_NUM_(R13,  i * 8);
        MOV_MR_R_(R13, R12);
    }

    ADD_R_NUM_(RBX, (int)size * 8);
    MOV_R1_R2_(R14, RBX);
    SUB_R_NUM_(R14, 16);

    CreateAsmFileX86(language, tree_node->right_node, asm_file, bin_file);

    free(language->array_with_variables.data);

    fprintf(asm_file, ";/ end def function\n\n");
}

void PrintCallFunctionX86(Language* language, Tree_node* tree_node, FILE* asm_file, FILE* bin_file) {
    assert(language);
    assert(tree_node);
    assert(asm_file);

    fprintf(asm_file, ";/ begin call function:\n");

    Tree_node* param_node = tree_node->right_node;

    while (param_node != NULL 
           && param_node->type == OPERATOR && param_node->value.operators == OPERATOR_PARAM) {
        CreateAsmFileX86(language, param_node->left_node, asm_file, bin_file);

        param_node = param_node->right_node;
    }

    if (param_node != NULL) {
        size_t size = language->array_with_variables.size;
        for (size_t i = 0; i < size; ++i) {
            About_variable about_variable = {};
            ArrayGetElement(&language->array_with_variables, &about_variable, i);

            if (strcmp(about_variable.name, NameOfVariable(param_node)) == 0) {
                MOV_R1_R2_(R12, RBP);
                SUB_R1_R2_(R12, RBX);
                ADD_R_NUM_(R12, (int)(size - i) * 8);
                PUSH_M_(R12);
            }
        }
    }

    Tree_node* name_node = tree_node->left_node;
    CALL_(NameOfVariable(name_node));
    PUSH_R_(RAX);

    fprintf(asm_file, ";/ end call function\n\n");
}

void PrintReturnFunctionX86(Language* language, Tree_node* tree_node, FILE* asm_file, FILE* bin_file) {
    assert(language);
    assert(tree_node);
    assert(asm_file);

    fprintf(asm_file, ";/ begin return:\n");

    CreateAsmFileX86(language, tree_node->left_node, asm_file, bin_file);

    POP_R_(RAX);
    MOV_R1_R2_(R12, RBX);
    SUB_R_NUM_(R12, 16);
    ADD_R1_R2_(RSP, R12);
    POP_R_(RBX);
    MOV_R1_R2_(RSP, RBP);
    POP_R_(RBP);
    POP_R_(R12);
    POP_R_(R13);
    ADD_R1_R2_(RSP, R14);
    MOV_R1_R2_(R14, R12);
    PUSH_R_(R13);
    RET_;

    fprintf(asm_file, ";/ end return\n\n");
}

void PrintFuncMyPrintf(Language* language, FILE* asm_file, FILE* bin_file) {
    assert(language);
    assert(asm_file);

    #define L_(name) "mprintf_" #name
    #define DOT_L_(name) "my_printf.mprintf_" #name

    FUNC_("my_printf");

    #define SAVE_IN_STACK_(reg, n)  \
        MOV_R1_R2_(R11, RBP);       \
        SUB_R_NUM_(R11, 8 * n);     \
        MOV_MR_R_(R11, reg)
        

    PUSH_R_(RBP); MOV_R1_R2_(RBP, RSP); PUSH_R_(R12); PUSH_R_(R13); PUSH_R_(R14); PUSH_R_(RBX);
    SUB_R_NUM_(RSP, 40);
    SAVE_IN_STACK_(RSI, 5); SAVE_IN_STACK_(RDX, 6); SAVE_IN_STACK_(RCX, 7); SAVE_IN_STACK_(R8, 8); SAVE_IN_STACK_(R9, 9);

    MOV_R1_R2_(R12, RDI); XOR_R1_R2_(R13, R13); XOR_R1_R2_(R14, R14);
    DOT_LABEL_(check_symbs);

    MOV_R8_MR_(AL, R12); CMP_R8_CHAR(AL, 0); JE_(DOT_L_(done), GetLabelAddress(DOT_L_(done)));
    CMP_R8_CHAR(AL, '%'); JE_(DOT_L_(specificator), GetLabelAddress(DOT_L_(specificator)));
    CALL_(L_(print_char)); INC_(R12); JMP_(DOT_L_(check_symbs), GetLabelAddress(DOT_L_(check_symbs)));

    DOT_LABEL_(next_symb); INC_(R12); JMP_(DOT_L_(check_symbs), GetLabelAddress(DOT_L_(check_symbs)));

    DOT_LABEL_(specificator);
    INC_(R12); MOV_R8_MR_(AL, R12);
    CMP_R8_CHAR(AL, 0); JE_(DOT_L_(done), GetLabelAddress(DOT_L_(done)));
    CMP_R8_CHAR(AL, '%'); JE_(L_(case_percent), GetLabelAddress(L_(case_percent)));
    CMP_R8_CHAR(AL, 'b'); JL_(L_(case_default), GetLabelAddress(L_(case_default)));
    CMP_R8_CHAR(AL, 'x'); JG_(L_(case_default), GetLabelAddress(L_(case_default)));

    MOVZX_R1_R2_(RBX, AL); MOV_R_ADDR_(R11, ADDR_JMP_TABLE); PUSH_R_(RBX); SUB_R_NUM_(RBX, 'b'); SHL_(RBX, 3); ADD_R1_R2_(R11, RBX); POP_R_(RBX); MOV_R_MR_(R11, R11); JMP_R_(R11);
    DOT_LABEL_(done);
    CALL_(L_(print_buf_to_console));
    ADD_R_NUM_(RSP, 40); POP_R_(RBX); POP_R_(R14); POP_R_(R13); POP_R_(R12); POP_R_(RBP); RET_;

    FUNC_("mprintf_case_percent"); MOV_R8_CHAR_(AL, '%'); CALL_(L_(print_char)); JMP_(DOT_L_(next_symb), GetLabelAddress(DOT_L_(next_symb)));
    FUNC_("mprintf_case_b"); CALL_(L_(get_arg)); MOV_R_NUM_(RCX, 1); MOV_R_NUM_(RSI, 1); MOV_R_NUM_(R9, 64); CALL_(L_(print_deg_2)); INC_(R13); JMP_(DOT_L_(next_symb), GetLabelAddress(DOT_L_(next_symb)));
    FUNC_("mprintf_case_c"); CALL_(L_(get_arg)); CALL_(L_(print_char)); INC_(R13); JMP_(DOT_L_(next_symb), GetLabelAddress(DOT_L_(next_symb)));
    FUNC_("mprintf_case_d"); CALL_(L_(get_arg)); MOVSXD_R1_R2_(RAX, EAX); CALL_(L_(print_decimal)); INC_(R13); JMP_(DOT_L_(next_symb), GetLabelAddress(DOT_L_(next_symb)));
    FUNC_("mprintf_case_f"); MOV_R_ADDR_(R11, ADDR_XMM_SAVE); PUSH_R_(R14); IMUL_R_NUM_(R14, 8); ADD_R1_R2_(R11, R14); MOVQ_R_M_(XMM0, R11); POP_R_(R14);CALL_(L_(print_float)); INC_(R14);JMP_(DOT_L_(next_symb), GetLabelAddress(DOT_L_(next_symb)));
    FUNC_("mprintf_case_o"); CALL_(L_(get_arg)); MOV_R1_R2_(RBX, RAX); SHL_(RBX, 1); MOV_R1_R2_(RAX, RBX); MOV_R_NUM_(RCX, 3); MOV_R_NUM_(RSI, 7); MOV_R_NUM_(R9, 21); CALL_(L_(print_deg_2)); INC_(R13); JMP_(DOT_L_(next_symb), GetLabelAddress(DOT_L_(next_symb)));
    FUNC_("mprintf_case_s"); CALL_(L_(get_arg)); MOV_R1_R2_(RDI, RAX); CALL_(L_(print_string)); INC_(R13); JMP_(DOT_L_(next_symb), GetLabelAddress(DOT_L_(next_symb)));
    FUNC_("mprintf_case_x"); CALL_(L_(get_arg)); MOV_R_NUM_(RCX, 4); MOV_R_NUM_(RSI, 15); MOV_R_NUM_(R9, 16); CALL_(L_(print_deg_2)); INC_(R13); JMP_(DOT_L_(next_symb), GetLabelAddress(DOT_L_(next_symb))); 
    FUNC_("mprintf_case_default"); CALL_(L_(print_char)); JMP_(DOT_L_(next_symb), GetLabelAddress(DOT_L_(next_symb)));

    FUNC_("mprintf_get_arg"); CMP_R_NUM_(R13, 5); JL_(DOT_L_(arg_in_regs), GetLabelAddress(DOT_L_(arg_in_regs)));
    DOT_LABEL_(arg_in_stack); MOV_R1_R2_(RBX, R13); SUB_R_NUM_(RBX, 5); MOV_R1_R2_(R11, RBX); SHL_(R11, 3); ADD_R_NUM_(R11, 16); ADD_R1_R2_(R11, RBP); MOV_R1_R2_(RBX, R11); MOV_R_MR_(RAX, RBX); RET_;
    DOT_LABEL_(arg_in_regs); MOV_R1_R2_(RAX, R13); SHL_(RAX, 3); NEG_(RAX); ADD_R1_R2_(RAX, RBP); SUB_R_NUM_(RAX, 40); MOV_R_MR_(RAX, RAX); RET_;

    FUNC_("mprintf_print_char"); PUSH_R_(RAX); PUSH_R_(RSI); PUSH_R_(RDI); PUSH_R_(RDX); PUSH_R_(R11);
    MOV_R_ADDR_(R11, ADDR_BUF_LEN); MOV_R_MR_(RDX, R11); CMP_R_NUM_(RDX, 1024); JNE_(DOT_L_(print_to_buf), GetLabelAddress(DOT_L_(print_to_buf))); CALL_(L_(print_buf_to_console));
    DOT_LABEL_(print_to_buf);  
    MOV_R_ADDR_(R11, ADDR_BUF); ADD_R1_R2_(R11, RDX); MOV_MR_R8_(R11, AL); INC_(RDX); MOV_R_ADDR_(R11, ADDR_BUF_LEN); MOV_MR_R_(R11, RDX);
    POP_R_(R11); POP_R_(RDX); POP_R_(RDI); POP_R_(RSI); POP_R_(RAX); RET_;
    
    FUNC_("mprintf_print_buf_to_console"); PUSH_R_(RAX); PUSH_R_(RDI); PUSH_R_(RSI); PUSH_R_(RDX); PUSH_R_(R11); PUSH_R_(RCX);
    MOV_R_ADDR_(R11, ADDR_BUF_LEN); MOV_R_MR_(RDX, R11); CMP_R_NUM_(RDX, 0); JE_(DOT_L_(pbc_done), GetLabelAddress(DOT_L_(pbc_done))); MOV_R_NUM_(RAX, 1); MOV_R_NUM_(RDI, 1); MOV_R_ADDR_(RSI, ADDR_BUF); SYSCALL_; 
    XOR_R1_R2_(RDX, RDX); MOV_R_ADDR_(R11, ADDR_BUF_LEN); MOV_MR_R_(R11, RDX);
    DOT_LABEL_(pbc_done); POP_R_(RCX); POP_R_(R11); POP_R_(RDX); POP_R_(RSI); POP_R_(RDI); POP_R_(RAX); RET_;

    FUNC_("mprintf_print_decimal"); PUSH_R_(RBX); TEST_R1_R2_(RAX, RAX); JNS_(DOT_L_(positive_num), GetLabelAddress(DOT_L_(positive_num))); PUSH_R_(RAX); MOV_R8_CHAR_(AL, '-'); CALL_(L_(print_char)); POP_R_(RAX); NEG_(RAX);
    DOT_LABEL_(positive_num); MOV_R_NUM_(RBX, 10); XOR_R1_R2_(RCX, RCX);
    DOT_LABEL_(div_10); XOR_R1_R2_(RDX, RDX); DIV_R_(RBX); ADD_R_NUM_(RDX, '0'); PUSH_R_(RDX); INC_(RCX); CMP_R_NUM_(RAX, 0); JNE_(DOT_L_(div_10), GetLabelAddress(DOT_L_(div_10)));
    DOT_LABEL_(print_loop); POP_R_(RAX); PUSH_R_(RCX); CALL_(L_(print_char)); POP_R_(RCX); LOOP_(DOT_L_(print_loop), GetLabelAddress(DOT_L_(print_loop)));
    POP_R_(RBX); RET_;

    FUNC_("mprintf_print_float"); RET_;

    FUNC_("mprintf_print_string"); PUSH_R_(RSI); MOV_R1_R2_(RSI, RDI);
    DOT_LABEL_(ps_loop); MOV_R8_MR_(AL, RSI); CMP_R8_CHAR(AL, 0); JE_(DOT_L_(ps_done), GetLabelAddress(DOT_L_(ps_done)));
    CALL_(L_(print_char)); INC_(RSI); JMP_(DOT_L_(ps_loop), GetLabelAddress(DOT_L_(ps_loop)));
    DOT_LABEL_(ps_done); POP_R_(RSI); RET_;

    FUNC_("mprintf_print_deg_2"); PUSH_R_(RBX); PUSH_R_(RCX); PUSH_R_(RDX); PUSH_R_(R11); PUSH_R_(RSI);
    MOV_R1_R2_(RBX, RAX); XOR_R1_R2_(R11, R11); MOV_R1_R2_(RDX, RCX);
    DOT_LABEL_(deg_loop); ROL_R_CL_(RBX); MOV_R1_R2_(RAX, RBX); AND_R1_R2_(RAX, RSI);
    JZ_(DOT_L_(zero_digit), GetLabelAddress(DOT_L_(zero_digit))); MOV_R_NUM_(R11, 1); JMP_(DOT_L_(print_digit), GetLabelAddress(DOT_L_(print_digit)));
    DOT_LABEL_(zero_digit); CMP_R_NUM_(R11, 1); JE_(DOT_L_(print_digit), GetLabelAddress(DOT_L_(print_digit))); 
    CMP_R_NUM_(R9, 1); JE_(DOT_L_(print_digit), GetLabelAddress(DOT_L_(print_digit))); JMP_(DOT_L_(next_digit), GetLabelAddress(DOT_L_(next_digit)));
    DOT_LABEL_(print_digit); MOV_R_ADDR_(R10, ADDR_HEX_CHARS); ADD_R1_R2_(R10, RAX); MOV_R8_MR_(AL, R10); CALL_(L_(print_char));
    DOT_LABEL_(next_digit); DEC_(R9); JNZ_(DOT_L_(deg_loop), GetLabelAddress(DOT_L_(deg_loop)));
    POP_R_(RSI); POP_R_(R11); POP_R_(RDX); POP_R_(RCX); POP_R_(RBX); RET_;

    #undef L_
    #undef DOT_L_
    #undef SAVE_IN_STACK
}

void PrintFuncMyScanf(Language* language, FILE* asm_file, FILE* bin_file) {
    assert(language);
    assert(asm_file);

    #define L_(name) "mscanf_" #name
    #define DOT_L_(name) "mscanf_" #name

    FUNC_("my_scanf");

    PUSH_R_(RBP); MOV_R1_R2_(RBP, RSP); PUSH_R_(RBX); PUSH_R_(R12); PUSH_R_(R13);
    MOV_R1_R2_(RBX, RSI); XOR_R1_R2_(R13, R13);
    MOV_R_NUM_(RAX, 0); MOV_R_NUM_(RDI, 0); MOV_R_ADDR_(RSI, ADDR_BUFFER); MOV_R_NUM_(RDX, 32); SYSCALL_;
    MOV_R_ADDR_(RSI, ADDR_BUFFER); XOR_R1_R2_(RAX, RAX);
    DOT_LABEL_(check_sign); MOV_R8_MR_(CL, RSI); CMP_R8_CHAR(CL, '-'); JNE_(DOT_L_(loop), GetLabelAddress(DOT_L_(loop))); MOV_R_NUM_(R13, 1); INC_(RSI);
    DOT_LABEL_(loop); MOVZX_R_M_(RCX, RSI); 
    CMP_R8_CHAR(CL, '0'); JL_(DOT_L_(apply_sign), GetLabelAddress(DOT_L_(apply_sign))); CMP_R8_CHAR(CL, '9'); JG_(DOT_L_(apply_sign), GetLabelAddress(DOT_L_(apply_sign)));
    SUB_R_NUM_(RCX, '0'); IMUL_R_NUM_(RAX, 10); ADD_R1_R2_(RAX, RCX); INC_(RSI); JMP_(DOT_L_(loop), GetLabelAddress(DOT_L_(loop)));

    DOT_LABEL_(apply_sign); TEST_R1_R2_(R13, R13); JZ_(DOT_L_(store), GetLabelAddress(DOT_L_(store))); NEG_(RAX);
    DOT_LABEL_(store); MOV_MR_R_(RBX, RAX); 
    POP_R_(R13); POP_R_(R12); POP_R_(RBX); POP_R_(RBP); RET_;

    #undef L_
    #undef DOT_L_
}