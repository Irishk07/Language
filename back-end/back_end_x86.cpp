#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "back_end_x86.h"

#include "../array.h"
#include "../common.h"
#include "common_back.h"
#include "onegin.h"
#include "../tree.h"


// objdump, readelf



Tree_status BackEndX86(Language* language, const char* name_asm_file) {
    assert(language);
    assert(name_asm_file);

    ReadPreOrderTreeFile(language);

    FILE* asm_file = fopen(name_asm_file, "w");
    if (asm_file == NULL)
        TREE_CHECK_AND_RETURN_ERRORS(OPEN_ERROR);

    fprintf(asm_file, "default rel\n\n");

    SECTION_DATA_

    SECTION_BSS_

    fprintf(asm_file, "section .text\n\n");
    MACRO_
    fprintf(asm_file, "global main\n\n"
                      "main:\n");

    CreateAsmFileX86(language, language->tree.root, asm_file);

    PrintFuncMyPrintf(language, asm_file);
    PrintFuncMyScanf(language, asm_file);

    fprintf(asm_file, "section .note.GNU-stack noalloc noexec nowrite progbits\n");

    if (fclose(asm_file) == EOF)
        TREE_CHECK_AND_RETURN_ERRORS(CLOSE_ERROR);

    return SUCCESS;
}

void CreateAsmFileX86(Language* language, Tree_node* tree_node, FILE* asm_file) {
    assert(language);
    assert(asm_file);

    if (tree_node == NULL)
        return;

    if (tree_node->type == NUMBER)
        PUSH_NUM_(tree_node->value.number);

    if (tree_node->type == VARIABLE) {
        size_t size = language->array_with_variables.size;
        for (size_t i = 0; i < language->array_with_variables.size; ++i) {
            if (strcmp(NameOfVariable(tree_node), NameOfVariableFromIndex(&language->array_with_variables, i)) == 0) {
                MOV_R1_R2_(r12, rbp);
                SUB_R1_R2_(r12, rbx);
                ADD_R_NUM_(r12, (int)(size - i) * 8);
                PUSH_M_(r12);
            }
        }   
    }            

    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wswitch-enum"
    if (tree_node->type == OPERATOR) {
        switch(tree_node->value.operators) {
            case OPERATOR_ADD:    PrintOperationsX86(language, tree_node, asm_file, "add");      break;
            case OPERATOR_SUB:    PrintOperationsX86(language, tree_node, asm_file, "sub");      break;
            case OPERATOR_MUL:    PrintOperationsX86(language, tree_node, asm_file, "imul");     break;    
            case OPERATOR_DIV:    PrintOperationDivX86(language, tree_node, asm_file);           break;   
            case OPERATOR_SQRT:   PrintOperationSqrtX86(language, tree_node, asm_file);          break;  

            case OPERATOR_ASSIGNMENT: PrintAssignmentX86(language, tree_node, asm_file);         break;
            case OPERATOR_CHANGE:     PrintChangeX86(language, tree_node, asm_file);             break;

            case OPERATOR_IF:         PrintIfX86(language, tree_node, asm_file);                 break;
            case OPERATOR_ELSE:                                                                  break;
            case OPERATOR_WHILE:      PrintWhileX86(language, tree_node, asm_file);              break;

            case OPERATOR_COMMON:
            case OPERATOR_PARAM:  CreateAsmFileX86(language, tree_node->left_node, asm_file);
                                  CreateAsmFileX86(language, tree_node->right_node, asm_file);   break;

            case OPERATOR_INPUT:  PrintInX86(language, tree_node, asm_file);                     break;
            case OPERATOR_PRINT:  PrintOutX86(language, tree_node, asm_file);                    break;

            case OPERATOR_MAIN_FUNCTION: PrintMainFunctionX86(language, tree_node, asm_file);    break;
            case OPERATOR_DEF_FUNCTION:  PrintDefFunctionX86(language, tree_node, asm_file);     break;
            case OPERATOR_CALL_FUNCTION: PrintCallFunctionX86(language, tree_node, asm_file);    break;
            case OPERATOR_RETURN:        PrintReturnFunctionX86(language, tree_node, asm_file);  break;

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

void PrintOperationsX86(Language* language, Tree_node* tree_node, FILE* asm_file, const char* operation) {
    assert(language);
    assert(asm_file);
    assert(tree_node);
    assert(operation);

    fprintf(asm_file, ";/ begin operation %s:\n", operation);

    CreateAsmFileX86(language, tree_node->left_node, asm_file);
    CreateAsmFileX86(language, tree_node->right_node, asm_file);

    POP_R_(r13);
    POP_R_(r12);
    fprintf(asm_file, "    %s r12, r13\n", operation);
    PUSH_R_(r12);

    fprintf(asm_file, ";/ end operation %s\n\n", operation);
}

void PrintOperationDivX86(Language* language, Tree_node* tree_node, FILE* asm_file) {
    assert(language);
    assert(asm_file);
    assert(tree_node);

    PUSH_R_(rax);
    PUSH_R_(rdx);
    PUSH_R_(rbx);

    fprintf(asm_file, ";/ begin operation idiv:\n");

    CreateAsmFileX86(language, tree_node->left_node, asm_file);
    CreateAsmFileX86(language, tree_node->right_node, asm_file);

    POP_R_(rbx);
    POP_R_(rax);
    CQO_;
    IDIV_R_(rbx);
    MOV_R1_R2_(r12, rax);
    POP_R_(rbx);
    POP_R_(rdx);
    POP_R_(rax);
    PUSH_R_(r12);

    fprintf(asm_file, ";/ end operation idiv\n\n");
}

void PrintOperationSqrtX86(Language* language, Tree_node* tree_node, FILE* asm_file) {
    assert(language);
    assert(asm_file);
    assert(tree_node);

    fprintf(asm_file, ";/ begin operation sqrt:\n");

    CreateAsmFileX86(language, tree_node->left_node, asm_file);

    POP_R_(r12);
    CVTSI2SD_(xmm0, r12);
    SQRTSD_(xmm1, xmm0);
    CVTTSD2SI_(r12, xmm1);
    PUSH_R_(r12);

    fprintf(asm_file, ";/ end operation sqrt\n\n");
}

void PrintAssignmentX86(Language* language, Tree_node* tree_node, FILE* asm_file) {
    assert(language);
    assert(asm_file);
    assert(tree_node);

    fprintf(asm_file, ";/ begin assignment:\n");

    CreateAsmFileX86(language, tree_node->right_node, asm_file);

    Tree_node* name_node = tree_node->left_node;

    About_variable about_variable = {.name = NameOfVariable(name_node), .value = (int)language->array_with_variables.size};
    
    POP_R_(r12);
    SUB_R_NUM_(rsp, 8);
    MOV_R1_R2_(rax, rbp);
    SUB_R1_R2_(rax, rbx);
    MOV_M_R_(rax, r12);
    
    ArrayPush(&language->array_with_variables, &about_variable);

    ADD_R_NUM_(rbx, 8);

    fprintf(asm_file, ";/ end assignment\n\n");
}

void PrintChangeX86(Language* language, Tree_node* tree_node, FILE* asm_file) {
    assert(language);
    assert(asm_file);
    assert(tree_node);

    fprintf(asm_file, ";/ begin change:\n");

    CreateAsmFileX86(language, tree_node->right_node, asm_file);

    size_t size = language->array_with_variables.size;
    for (size_t i = 0; i < language->array_with_variables.size; ++i) {
        if (strcmp(NameOfVariable(tree_node->left_node), NameOfVariableFromIndex(&language->array_with_variables, i)) == 0) {
            MOV_R1_R2_(r12, rbp);
            SUB_R1_R2_(r12, rbx);
            ADD_R_NUM_(r12, (int)(size - i) * 8);
            POP_M_(r12);
        }
    }         

    fprintf(asm_file, ";/ end change\n\n");
}

void PrintIfX86(Language* language, Tree_node* tree_node, FILE* asm_file) {
    assert(language);
    assert(asm_file);
    assert(tree_node);

    fprintf(asm_file, ";/ begin if_%zu:\n", language->cnt_labels.cnt_if);

    Tree_node* condition = tree_node->left_node;

    if (condition->type == OPERATOR && (condition->value.operators == OPERATOR_ABOVE ||
                                        condition->value.operators == OPERATOR_BEFORE ||
                                        condition->value.operators == OPERATOR_EQUAL)) {
        Type_operators type_operator = condition->value.operators;

        CreateAsmFileX86(language, condition->left_node, asm_file);
        CreateAsmFileX86(language, condition->right_node, asm_file);

        POP_R_(r13);
        POP_R_(r12);
        CMP_R1_R2_(r12, r13);

        if (type_operator == OPERATOR_ABOVE)
            JLE_
        else if (type_operator == OPERATOR_BEFORE)
            JGE_
        if (type_operator == OPERATOR_EQUAL)
            JNE_
    }
    else {
        CreateAsmFileX86(language, condition, asm_file);
        PUSH_NUM_(0);
        JE_
    }

    if (tree_node->right_node->type != OPERATOR || tree_node->right_node->value.operators != OPERATOR_ELSE) {
        fprintf(asm_file, ".end_if_%zu\n", language->cnt_labels.cnt_if);
        CreateAsmFileX86(language, tree_node->right_node, asm_file);
    }    
    else
        PrintElseX86(language, tree_node->right_node, asm_file);

    fprintf(asm_file, ".end_if_%zu:\n", language->cnt_labels.cnt_if);

    fprintf(asm_file, ";/ end if_%zu\n\n", language->cnt_labels.cnt_if);

    language->cnt_labels.cnt_if++;
}

void PrintElseX86(Language* language, Tree_node* tree_node, FILE* asm_file) {
    assert(language);
    assert(asm_file);
    assert(tree_node);

    fprintf(asm_file, ".else_%zu\n", language->cnt_labels.cnt_else);

    CreateAsmFileX86(language, tree_node->left_node, asm_file); // in if
    fprintf(asm_file, "    jmp .end_if_%zu\n", language->cnt_labels.cnt_if);
    fprintf(asm_file, ".else_%zu:\n", language->cnt_labels.cnt_else);
    CreateAsmFileX86(language, tree_node->right_node, asm_file); // in else

    fprintf(asm_file, ";/ end else_%zu\n\n", language->cnt_labels.cnt_else++);
}

void PrintWhileX86(Language* language, Tree_node* tree_node, FILE* asm_file) {
    assert(language);
    assert(asm_file);
    assert(tree_node);

    fprintf(asm_file, ";/ begin while_%zu:\n", language->cnt_labels.cnt_while);

    size_t cnt_while = language->cnt_labels.cnt_while;
    fprintf(asm_file, ".begin_while_%zu:\n", cnt_while);
    language->cnt_labels.cnt_while++;

    Tree_node* condition = tree_node->left_node;

    if (condition->type == OPERATOR && (condition->value.operators == OPERATOR_ABOVE ||
                                        condition->value.operators == OPERATOR_BEFORE ||
                                        condition->value.operators == OPERATOR_EQUAL)) {
        Type_operators type_operator = condition->value.operators;

        CreateAsmFileX86(language, condition->left_node, asm_file);
        CreateAsmFileX86(language, condition->right_node, asm_file);

        POP_R_(r13);
        POP_R_(r12);
        CMP_R1_R2_(r12, r13);

        if (type_operator == OPERATOR_ABOVE)
            JLE_
        else if (type_operator == OPERATOR_BEFORE)
            JGE_
        if (type_operator == OPERATOR_EQUAL)
            JNE_
    }
    else {
        CreateAsmFileX86(language, condition, asm_file);
        PUSH_NUM_(0);
        JE_
    }
    fprintf(asm_file, ".end_while_%zu\n", cnt_while);

    CreateAsmFileX86(language, tree_node->right_node, asm_file);
    fprintf(asm_file, "jmp .begin_while_%zu\n", cnt_while);

    fprintf(asm_file, ".end_while_%zu:\n", cnt_while);

    fprintf(asm_file, ";/ end while_%zu\n\n", cnt_while);
}

void PrintInX86(Language* language, Tree_node* tree_node, FILE* asm_file) {
    assert(language);
    assert(tree_node);
    assert(asm_file);

    fprintf(asm_file, ";/ begin in:\n");

    CreateAsmFileX86(language, tree_node->left_node, asm_file);

    SUB_R_NUM_(rsp, 8);
    MOV_R1_R2_(r12, rsp);
    AND_R_NUM_(rsp, -16);
    MOV_FORMAT_IN_(rdi);
    MOV_R1_R2_(rsi, rbp);
    SUB_R1_R2_(rsi, rbx);
    XOR_R1_R2_(rax, rax);
    CALL_SCANF_;
    MOV_R1_R2_(rsp, r12);

    fprintf(asm_file, ";/ end in\n\n");
}

void PrintOutX86(Language* language, Tree_node* tree_node, FILE* asm_file) {
    assert(language);
    assert(tree_node);
    assert(asm_file);

    fprintf(asm_file, ";/ begin out:\n");

    CreateAsmFileX86(language, tree_node->left_node, asm_file);

    POP_R_(rsi);
    MOV_R1_R2_(r12, rsp);
    AND_R_NUM_(rsp, -16);
    MOV_FORMAT_OUT_(rdi);
    MOV_R_NUM_(rax, 0);
    CALL_PRINTF_;
    MOV_R1_R2_(rsp, r12);

    fprintf(asm_file, ";/ end out\n\n");
}

void PrintMainFunctionX86(Language* language, Tree_node* tree_node, FILE* asm_file) {
    assert(language);
    assert(tree_node);
    assert(asm_file);

    fprintf(asm_file, ";/ begin main:\n");

    language->array_with_variables = {};
    ArrayCtor(&(language->array_with_variables), sizeof(About_variable), DEFAULT_START_CAPACITY);

    PUSH_R_(rbp);
    MOV_R1_R2_(rbp, rsp);
    MOV_R_NUM_(rbx, 8);

    CreateAsmFileX86(language, tree_node->right_node, asm_file);

    free(language->array_with_variables.data);

    MOV_R1_R2_(rsp, rbp);
    POP_R_(rbp);
    MOV_R_NUM_(rax, 60);
    XOR_R1_R2_(rdi, rdi);
    SYSCALL_;

    fprintf(asm_file, ";/ end main\n\n");
}

void PrintDefFunctionX86(Language* language, Tree_node* tree_node, FILE* asm_file) {
    assert(language);
    assert(tree_node);
    assert(asm_file);

    fprintf(asm_file, ";/ begin def function:\n");

    Tree_node* common_node = tree_node->left_node;
    Tree_node* name_node = common_node->left_node;

    language->array_with_variables = {};
    ArrayCtor(&(language->array_with_variables), sizeof(About_variable), DEFAULT_START_CAPACITY);

    fprintf(asm_file, "\n%s:\n", NameOfVariable(name_node));

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

    PUSH_R_(r14);
    PUSH_R_(rbp);
    MOV_R1_R2_(rbp, rsp);
    PUSH_R_(rbx);
    MOV_R_NUM_(rbx, 16);

    size_t size = language->array_with_variables.size;
    SUB_R_NUM_(rsp, (int)size * 8);
    for (int i = (int)size - 1; i >= 0; --i) {
        fprintf(asm_file, "    mov r12, [rbp + %d]\n", ((int)size - i + 2) * 8);
        MOV_R1_R2_(r13, rbp);
        SUB_R1_R2_(r13, rbx);
        fprintf(asm_file, "    mov [r13 - %d], r12\n", i * 8);
    }

    ADD_R_NUM_(rbx, (int)size * 8);
    MOV_R1_R2_(r14, rbx);
    SUB_R_NUM_(r14, 16);

    CreateAsmFileX86(language, tree_node->right_node, asm_file);

    free(language->array_with_variables.data);

    fprintf(asm_file, ";/ end def function\n\n");
}

void PrintCallFunctionX86(Language* language, Tree_node* tree_node, FILE* asm_file) {
    assert(language);
    assert(tree_node);
    assert(asm_file);

    fprintf(asm_file, ";/ begin call function:\n");

    Tree_node* param_node = tree_node->right_node;

    while (param_node != NULL 
           && param_node->type == OPERATOR && param_node->value.operators == OPERATOR_PARAM) {
        CreateAsmFileX86(language, param_node->left_node, asm_file);

        param_node = param_node->right_node;
    }

    if (param_node != NULL) {
        size_t size = language->array_with_variables.size;
        for (size_t i = 0; i < size; ++i) {
            About_variable about_variable = {};
            ArrayGetElement(&language->array_with_variables, &about_variable, i);

            if (strcmp(about_variable.name, NameOfVariable(param_node)) == 0) {
                MOV_R1_R2_(r12, rbp);
                SUB_R1_R2_(r12, rbx);
                ADD_R_NUM_(r12, (int)(size - i) * 8);
                PUSH_M_(r12);
            }
        }
    }

    Tree_node* name_node = tree_node->left_node;
    CALL_(NameOfVariable(name_node));
    PUSH_R_(rax);

    fprintf(asm_file, ";/ end call function\n\n");
}

void PrintReturnFunctionX86(Language* language, Tree_node* tree_node, FILE* asm_file) {
    assert(language);
    assert(tree_node);
    assert(asm_file);

    fprintf(asm_file, ";/ begin return:\n");

    CreateAsmFileX86(language, tree_node->left_node, asm_file);

    POP_R_(rax);
    MOV_R1_R2_(r12, rbx);
    SUB_R_NUM_(r12, 16);
    ADD_R1_R2_(rsp, r12);
    POP_R_(rbx);
    MOV_R1_R2_(rsp, rbp);
    POP_R_(rbp);
    POP_R_(r12);
    POP_R_(r13);
    ADD_R1_R2_(rsp, r14);
    MOV_R1_R2_(r14, r12);
    PUSH_R_(r13);
    RET_;

    fprintf(asm_file, ";/ end return\n\n");
}

void PrintFuncMyPrintf(Language* language, FILE* asm_file) {
    assert(language);
    assert(asm_file);

    FUNC_(my_printf);

    SAVE_FLOAT_(xmm0, 0); SAVE_FLOAT_(xmm1, 1); SAVE_FLOAT_(xmm2, 2); SAVE_FLOAT_(xmm3, 3);
    SAVE_FLOAT_(xmm4, 4); SAVE_FLOAT_(xmm5, 5); SAVE_FLOAT_(xmm6, 6); SAVE_FLOAT_(xmm7, 7);

    PUSH_R_(rbp); MOV_R1_R2_(rbp, rsp); PUSH_R_(r12); PUSH_R_(r13); PUSH_R_(r14); PUSH_R_(rbx);
    SUB_R_NUM_(rsp, 40);
    SAVE_IN_STACK_(rsi, 5); SAVE_IN_STACK_(rdx, 6); SAVE_IN_STACK_(rcx, 7); SAVE_IN_STACK_(r8, 8); SAVE_IN_STACK_(r9, 9);

    MOV_R1_R2_(r12, rdi); XOR_R1_R2_(r13, r13); XOR_R1_R2_(r14, r14);
    LABEL_(check_symbs); MOV_R_M_(al, r12); CMP_R_NUM_(al, 0); JE_ TO_LABEL_(done); CMP_R_CHAR(al, '%'); JE_ TO_LABEL_(specificator);
    CALL_("print_char"); INC_(r12); JMP_ TO_LABEL_(check_symbs);
    LABEL_(next_symb); INC_(r12); JMP_  TO_LABEL_(check_symbs);
    LABEL_(specificator); INC_(r12); MOV_R_M_(al, r12);
    CMP_R_NUM_(al, 0); JE_ TO_LABEL_(done);
    CMP_R_CHAR(al, '%'); JE_ TO_FUNC_(case_percent);
    CMP_R_CHAR(al, 'b'); JL_ TO_FUNC_(case_default);
    CMP_R_CHAR(al, 'x'); JG_ TO_FUNC_(case_default);
    MOVZX_R1_R2_(rbx, al); LEA_R_M_(r11, jmp_table); JMP_ TO_ADDR_([r11 + (rbx - 'b') * 8]);
    LABEL_(done); CALL_("print_buf_to_console");
    ADD_R_NUM_(rsp, 40); POP_R_(rbx); POP_R_(r14); POP_R_(r13); POP_R_(r12); POP_R_(rbp); RET_;

    FUNC_(case_percent); MOV_R_CHAR_(al, %); CALL_("print_char"); JMP_ TO_FUNC_(my_printf.next_symb);
    FUNC_(case_b); CALL_("get_arg"); MOV_R_NUM_(rcx, 1); MOV_R_NUM_(rsi, 1); MOV_R_NUM_(r9, 64); CALL_("print_deg_2"); INC_(r13); JMP_ TO_FUNC_(my_printf.next_symb); 
    FUNC_(case_c); CALL_("get_arg"); CALL_("print_char"); INC_(r13); JMP_ TO_FUNC_(my_printf.next_symb); 
    FUNC_(case_d); CALL_("get_arg"); MOVSXD_R1_R2_(rax, eax); CALL_("print_decimal"); INC_(r13); JMP_ TO_FUNC_(my_printf.next_symb); 
    FUNC_(case_f); LEA_R_M_(r11, xmm_save); MOVQ_R_M_(xmm0, r11 + r14 * 8); CALL_("print_float"); INC_(r14); JMP_ TO_FUNC_(my_printf.next_symb); 
    FUNC_(case_o); CALL_("get_arg"); MOV_R1_R2_(rbx, rax); SHL_(rbx, 1); MOV_R1_R2_(rax, rbx); MOV_R_NUM_(rcx, 3); MOV_R_NUM_(rsi, 7); MOV_R_NUM_(r9, 21); CALL_("print_deg_2"); INC_(r13); JMP_ TO_FUNC_(my_printf.next_symb); 
    FUNC_(case_s); CALL_("get_arg"); MOV_R1_R2_(rdi, rax); CALL_("print_string"); INC_(r13); JMP_ TO_FUNC_(my_printf.next_symb); 
    FUNC_(case_x); CALL_("get_arg"); MOV_R_NUM_(rcx, 4); MOV_R_NUM_(rsi, 15); MOV_R_NUM_(r9, 16); CALL_("print_deg_2"); INC_(r13); JMP_ TO_FUNC_(my_printf.next_symb); 
    FUNC_(case_default); CALL_("print_char"); JMP_ TO_FUNC_(my_printf.next_symb); 

    FUNC_(get_arg); CMP_R_NUM_(r13, 5); JL_ TO_LABEL_(arg_in_regs);
    LABEL_(arg_in_stack); MOV_R1_R2_(rbx, r13); SUB_R_NUM_(rbx, 5); LEA_R_M_(rbx, rbp + 16 + rbx * 8); MOV_R_M_(rax, rbx); RET_;
    LABEL_(arg_in_regs); MOV_R1_R2_(rax, r13); SHL_(rax, 3); NEG_(rax); ADD_R1_R2_(rax, rbp); SUB_R_NUM_(rax, 40); MOV_R_M_(rax, rax); RET_;

    FUNC_(print_char); PUSH_R_(rax); PUSH_R_(rsi); PUSH_R_(rdi); PUSH_R_(rdx); PUSH_R_(r11);
    MOV_R_M_(rdx, buf_len); CMP_R_CHAR(rdx, BUF_CAPACITY); JNE_ TO_LABEL_(print_to_buf); CALL_("print_buf_to_console");
    LABEL_(print_to_buf); LEA_R_M_(r11, buf); MOV_M_R_(r11 + rdx, al); INC_(rdx); MOV_M_R_(buf_len, rdx);
    POP_R_(r11); POP_R_(rdx); POP_R_(rdi); POP_R_(rsi); POP_R_(rax); RET_;
    FUNC_(print_buf_to_console); PUSH_R_(rax); PUSH_R_(rdi); PUSH_R_(rsi); PUSH_R_(rdx); PUSH_R_(r11); PUSH_R_(rcx);
    MOV_R_M_(rdx, buf_len); CMP_R_NUM_(rdx, 0); JE_ TO_LABEL_(done); MOV_R_NUM_(rax, 1); MOV_R_NUM_(rdi, 1); LEA_R_M_(rsi, buf); SYSCALL_; XOR_R1_R2_(rdx, rdx); MOV_M_R_(buf_len, rdx);
    LABEL_(done); POP_R_(rcx); POP_R_(r11); POP_R_(rdx); POP_R_(rsi); POP_R_(rdi); POP_R_(rax); RET_;

    FUNC_(print_decimal); PUSH_R_(rbx); TEST_R1_R2_(rax, rax); JNE_ TO_LABEL_(positive_num); PUSH_R_(rax); MOV_R_CHAR_(al, -); CALL_("print_char"); POP_R_(rax); NEG_(rax);
    LABEL_(positive_num); MOV_R_NUM_(rbx, 10); XOR_R1_R2_(rcx, rcx);
    LABEL_(div_10); XOR_R1_R2_(rdx, rdx); DIV_R_(rbx); ADD_R_NUM_(rdx, '0'); PUSH_R_(rdx); INC_(rcx); CMP_R_NUM_(rax, 0); JNE_ TO_LABEL_(div_10);
    LABEL_(print_loop); POP_R_(rax); PUSH_R_(rcx); CALL_("print_char"); POP_R_(rcx); LOOP_ TO_LABEL_(print_loop);
    POP_R_(rbx); RET_;

    FUNC_(print_float); RET_;

    FUNC_(print_string); PUSH_R_(rsi); MOV_R1_R2_(rsi, rdi);
    LABEL_(print_loop); MOV_R_M_(al, rsi); CMP_R_NUM_(al, 0); JE_ TO_LABEL_(done);
    CALL_("print_char"); INC_(rsi); JMP_ TO_LABEL_(print_loop);
    LABEL_(done); POP_R_(rsi); RET_;

    FUNC_(print_deg_2); PUSH_R_(rbx); PUSH_R_(rcx); PUSH_R_(rdx); PUSH_R_(r11); PUSH_R_(rsi);
    MOV_R1_R2_(rbx, rax); XOR_R1_R2_(r11, r11); MOV_R1_R2_(rdx, rcx);
    LABEL_(loop); ROL_R1_R2_(rbx, cl); MOV_R1_R2_(rax, rbx); AND_R1_R2_(rax, rsi);
    JZ_ TO_LABEL_(zero_digit); MOV_R_NUM_(r11, 1); JMP_ TO_LABEL_(print);
    LABEL_(zero_digit); CMP_R_NUM_(r11, 1); JE_ TO_LABEL_(print); CMP_R_NUM_(r9, 1); JE_ TO_LABEL_(print); JMP_ TO_LABEL_(next_digit);
    LABEL_(print); LEA_R_M_(r10, hex_chars); MOV_R_M_(al, r10 + rax); CALL_("print_char");
    LABEL_(next_digit); DEC_(r9); JNZ_ TO_LABEL_(loop);
    POP_R_(rsi); POP_R_(r11); POP_R_(rdx); POP_R_(rcx); POP_R_(rbx); RET_;
}

void PrintFuncMyScanf(Language* language, FILE* asm_file) {
    assert(language);
    assert(asm_file);

    FUNC_(my_scanf);

    PUSH_R_(rbp); MOV_R1_R2_(rbp, rsp); PUSH_R_(rbx); PUSH_R_(r12); PUSH_R_(r13);
    MOV_R1_R2_(rbx, rsi); XOR_R1_R2_(r13, r13);
    MOV_R_NUM_(rax, 0); MOV_R_NUM_(rdi, 0); LEA_R_M_(rsi, buffer); MOV_R_NUM_(rdx, 32); SYSCALL_;
    LEA_R_M_(rsi, buffer); XOR_R1_R2_(rax, rax);
    LABEL_(check_sign); CMP_R_CHAR(cl, '-'); JNE_ TO_LABEL_(loop); MOV_R_NUM_(r13, 1); INC_(rsi);
    LABEL_(loop); MOVZX_R_M_(rcx, rsi); CMP_R_CHAR(cl, '0'); JL_ TO_LABEL_(apply_sign); CMP_R_CHAR(cl, '9'); JG_ TO_LABEL_(apply_sign);
    SUB_R_NUM_(rcx, '0'); IMUL_R_NUM_(rax, 10); ADD_R1_R2_(rax, rcx); INC_(rsi); JMP_ TO_LABEL_(loop);

    LABEL_(apply_sign); TEST_R1_R2_(r13, r13); JZ_ TO_LABEL_(store); NEG_(rax);
    LABEL_(store); MOV_M_R_(rbx, rax); 
    POP_R_(r13); POP_R_(r12); POP_R_(rbx); POP_R_(rbp); RET_;
}