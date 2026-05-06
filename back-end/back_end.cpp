#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "back_end.h"

#include "../array.h"
#include "../common.h"
#include "common_back.h"
#include "onegin.h"
#include "../tree.h"


Tree_status BackEnd(Language* language, const char* name_asm_file) {
    assert(language);
    assert(name_asm_file);

    FILE* asm_file = fopen(name_asm_file, "w");
    if (asm_file == NULL)
        TREE_CHECK_AND_RETURN_ERRORS(OPEN_ERROR);

    fprintf(asm_file, "PUSH 0\n"
                      "POP RAX\n"
                      "PUSH 0\n"
                      "POP RBX\n"
                      "CALL :main\n\n");

    CreateAsmFile(language, language->tree.root, asm_file);

    fprintf(asm_file, ":end\n" 
                       "HLT\n");

    if (fclose(asm_file) == EOF)
        TREE_CHECK_AND_RETURN_ERRORS(CLOSE_ERROR);

    return SUCCESS;
}

void CreateAsmFile(Language* language, Tree_node* tree_node, FILE* asm_file) {
    assert(language);
    assert(asm_file);

    if (tree_node == NULL)
        return;

    if (tree_node->type == NUMBER)
        fprintf(asm_file, "PUSH %d\n", tree_node->value.number);

    if (tree_node->type == VARIABLE)
        for (size_t i = 0; i < language->array_with_variables.size; ++i) {
            if (strcmp(NameOfVariable(tree_node), NameOfVariableFromIndex(&language->array_with_variables, i)) == 0)
                fprintf(asm_file, "PUSH RBX\n"
                                  "PUSH %zu\n" 
                                  "ADD\n"
                                  "POP RCX\n"
                                  "PUSH [RCX]\n", i);
        }               

    if (tree_node->type == OPERATOR) {
        switch(tree_node->value.operators) {
            case OPERATOR_ADD:    PrintOperations(language, tree_node, asm_file, "ADD");      break;
            case OPERATOR_SUB:    PrintOperations(language, tree_node, asm_file, "SUB");      break;
            case OPERATOR_MUL:    PrintOperations(language, tree_node, asm_file, "MUL");      break;    
            case OPERATOR_DIV:    PrintOperations(language, tree_node, asm_file, "DIV");      break;   
            case OPERATOR_POW:    PrintOperations(language, tree_node, asm_file, "POW");      break; 

            case OPERATOR_LN:     PrintMathFunction(language, tree_node, asm_file, "LN");     break;    
            case OPERATOR_LOG:    PrintMathFunction(language, tree_node, asm_file, "LOG");    break;    
            case OPERATOR_SIN:    PrintMathFunction(language, tree_node, asm_file, "SIN");    break;   
            case OPERATOR_COS:    PrintMathFunction(language, tree_node, asm_file, "COS");    break;   
            case OPERATOR_TG:     PrintMathFunction(language, tree_node, asm_file, "TG");     break;   
            case OPERATOR_CTG:    PrintMathFunction(language, tree_node, asm_file, "CTG");    break;   
            case OPERATOR_ARCSIN: PrintMathFunction(language, tree_node, asm_file, "ARCSIN"); break; 
            case OPERATOR_ARCCOS: PrintMathFunction(language, tree_node, asm_file, "ARCCOS"); break;
            case OPERATOR_ARCTG:  PrintMathFunction(language, tree_node, asm_file, "ARCTG");  break;
            case OPERATOR_ARCCTG: PrintMathFunction(language, tree_node, asm_file, "ARCCTG"); break;
            case OPERATOR_SH:     PrintMathFunction(language, tree_node, asm_file, "SH");     break;    
            case OPERATOR_CH:     PrintMathFunction(language, tree_node, asm_file, "CH");     break;
            case OPERATOR_TH:     PrintMathFunction(language, tree_node, asm_file, "TH");     break;
            case OPERATOR_CTH:    PrintMathFunction(language, tree_node, asm_file, "CTH");    break;
            case OPERATOR_SQRT:   PrintMathFunction(language, tree_node, asm_file, "SQRT");   break;

            case OPERATOR_DRAW:   fprintf(asm_file, "DRAW\n");                                break;

            case OPERATOR_ASSIGNMENT: PrintAssignment(language, tree_node, asm_file);         break;
            case OPERATOR_CHANGE:     PrintChange(language, tree_node, asm_file);             break;

            case OPERATOR_IF:         PrintIf(language, tree_node, asm_file);                 break;
            case OPERATOR_ELSE:                                                               break;
            case OPERATOR_WHILE:      PrintWhile(language, tree_node, asm_file);              break;

            case OPERATOR_COMMON:
            case OPERATOR_PARAM:  CreateAsmFile(language, tree_node->left_node, asm_file);
                                  CreateAsmFile(language, tree_node->right_node, asm_file);   break;

            case OPERATOR_INPUT:  fprintf(asm_file, "IN\n");                                  break;
            case OPERATOR_PRINT:  PrintOut(language, tree_node, asm_file);                    break;

            case OPERATOR_MAIN_FUNCTION: PrintMainFunction(language, tree_node, asm_file);    break;
            case OPERATOR_DEF_FUNCTION:  PrintDefFunction(language, tree_node, asm_file);     break;
            case OPERATOR_CALL_FUNCTION: PrintCallFunction(language, tree_node, asm_file);    break;
            case OPERATOR_RETURN:        PrintReturnFunction(language, tree_node, asm_file);  break;

            case OPERATOR_C_ASSIGNMENT:
            case OPERATOR_C_CHANGE:
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
}

void PrintOperations(Language* language, Tree_node* tree_node, FILE* asm_file, const char* operation) {
    assert(language);
    assert(asm_file);
    assert(tree_node);
    assert(operation);

    fprintf(asm_file, "/ begin operation %s:\n", operation);

    CreateAsmFile(language, tree_node->left_node, asm_file);
    CreateAsmFile(language, tree_node->right_node, asm_file);

    fprintf(asm_file, "%s\n", operation);

    fprintf(asm_file, "/ end operation %s:\n\n", operation);
}

void PrintMathFunction(Language* language, Tree_node* tree_node, FILE* asm_file, const char* function) {
    assert(language);
    assert(asm_file);
    assert(tree_node);
    assert(function);

    fprintf(asm_file, "/ begin function %s:\n", function);

    CreateAsmFile(language, tree_node->left_node, asm_file);
    fprintf(asm_file, "%s\n", function);

    fprintf(asm_file, "/ end function %s:\n\n", function);
}

void PrintAssignment(Language* language, Tree_node* tree_node, FILE* asm_file) {
    assert(language);
    assert(asm_file);
    assert(tree_node);

    fprintf(asm_file, "/ begin assignment:\n");

    CreateAsmFile(language, tree_node->right_node, asm_file);

    Tree_node* name_node = tree_node->left_node;

    About_variable about_variable = {.name = NameOfVariable(name_node), .value = (int)language->array_with_variables.size};
    fprintf(asm_file, "POP [RAX]\n"); // TODO make const for RAX
    ArrayPush(&language->array_with_variables, &about_variable);

    fprintf(asm_file, "PUSH RAX\n"
                      "PUSH 1\n"
                      "ADD\n"
                      "POP RAX\n");

    fprintf(asm_file, "/ end assignment:\n\n");
}

void PrintChange(Language* language, Tree_node* tree_node, FILE* asm_file) {
    assert(language);
    assert(asm_file);
    assert(tree_node);

    fprintf(asm_file, "/ begin change:\n");

    CreateAsmFile(language, tree_node->right_node, asm_file);

    for (size_t i = 0; i < language->array_with_variables.size; ++i) {
        if (strcmp(NameOfVariable(tree_node->left_node), NameOfVariableFromIndex(&language->array_with_variables, i)) == 0)
            fprintf(asm_file, "PUSH RBX\n"
                              "PUSH %zu\n" 
                              "ADD\n"
                              "POP RCX\n"
                              "POP [RCX]\n", i); // TODO think
    }         

    fprintf(asm_file, "/ end change:\n\n");
}

void PrintIf(Language* language, Tree_node* tree_node, FILE* asm_file) {
    assert(language);
    assert(asm_file);
    assert(tree_node);

    fprintf(asm_file, "/ begin if:\n");

    Tree_node* condition = tree_node->left_node;

    if (condition->type == OPERATOR && (condition->value.operators == OPERATOR_ABOVE ||
                                        condition->value.operators == OPERATOR_BEFORE ||
                                        condition->value.operators == OPERATOR_EQUAL)) {
        Type_operators type_operator = condition->value.operators;

        CreateAsmFile(language, condition->left_node, asm_file);
        CreateAsmFile(language, condition->right_node, asm_file);

        if (type_operator == OPERATOR_ABOVE)
            fprintf(asm_file, "JBE ");
        else if (type_operator == OPERATOR_BEFORE)
            fprintf(asm_file, "JAE ");
        if (type_operator == OPERATOR_EQUAL)
            fprintf(asm_file, "JNE ");
    }
    else {
        CreateAsmFile(language, condition, asm_file);
        fprintf(asm_file, "PUSH 0\n"
                          "JE ");
    }

    if (tree_node->right_node->type != OPERATOR || tree_node->right_node->value.operators != OPERATOR_ELSE) {
        fprintf(asm_file, ":end_if_%zu\n", language->cnt_labels.cnt_if);
        CreateAsmFile(language, tree_node->right_node, asm_file);
    }    
    else
        PrintElse(language, tree_node->right_node, asm_file);

    fprintf(asm_file, ":end_if_%zu\n", language->cnt_labels.cnt_if);

    language->cnt_labels.cnt_if++;

    fprintf(asm_file, "/ end if:\n\n");
}

void PrintElse(Language* language, Tree_node* tree_node, FILE* asm_file) {
    assert(language);
    assert(asm_file);
    assert(tree_node);

    fprintf(asm_file, ":else_%zu\n", language->cnt_labels.cnt_else);

    CreateAsmFile(language, tree_node->left_node, asm_file); // in if
    fprintf(asm_file, "JMP :end_if_%zu\n", language->cnt_labels.cnt_if);
    fprintf(asm_file, ":else_%zu\n", language->cnt_labels.cnt_else);
    CreateAsmFile(language, tree_node->right_node, asm_file); // in else

    language->cnt_labels.cnt_else++;

    fprintf(asm_file, "/ end else:\n\n");
}

void PrintWhile(Language* language, Tree_node* tree_node, FILE* asm_file) {
    assert(language);
    assert(asm_file);
    assert(tree_node);

    fprintf(asm_file, "/ begin while:\n");

    size_t cnt_while = language->cnt_labels.cnt_while;
    fprintf(asm_file, ":begin_while_%zu\n", cnt_while);
    language->cnt_labels.cnt_while++;

    Tree_node* condition = tree_node->left_node;

    if (condition->type == OPERATOR && (condition->value.operators == OPERATOR_ABOVE ||
                                        condition->value.operators == OPERATOR_BEFORE ||
                                        condition->value.operators == OPERATOR_EQUAL)) {
        Type_operators type_operator = condition->value.operators;

        CreateAsmFile(language, condition->left_node, asm_file);
        CreateAsmFile(language, condition->right_node, asm_file);

        if (type_operator == OPERATOR_ABOVE)
            fprintf(asm_file, "JBE ");
        else if (type_operator == OPERATOR_BEFORE)
            fprintf(asm_file, "JAE ");
        if (type_operator == OPERATOR_EQUAL)
            fprintf(asm_file, "JNE ");
    }
    else {
        CreateAsmFile(language, condition, asm_file);
        fprintf(asm_file, "PUSH 0\n"
                          "JE ");
    }
    fprintf(asm_file, ":end_while_%zu\n", cnt_while);

    CreateAsmFile(language, tree_node->right_node, asm_file);
    fprintf(asm_file, "JMP :begin_while_%zu\n", cnt_while);

    fprintf(asm_file, ":end_while_%zu\n", cnt_while);

    fprintf(asm_file, "/ end while:\n\n");
}

void PrintOut(Language* language, Tree_node* tree_node, FILE* asm_file) {
    assert(language);
    assert(tree_node);
    assert(asm_file);

    fprintf(asm_file, "/ begin out:\n");

    CreateAsmFile(language, tree_node->left_node, asm_file);

    fprintf(asm_file, "OUT\n");

    fprintf(asm_file, "/ end out:\n\n");
}

void PrintMainFunction(Language* language, Tree_node* tree_node, FILE* asm_file) {
    assert(language);
    assert(tree_node);
    assert(asm_file);

    fprintf(asm_file, "/ begin main:\n");

    language->array_with_variables = {};
    ArrayCtor(&(language->array_with_variables), sizeof(About_variable), DEFAULT_START_CAPACITY);

    fprintf(asm_file, ":main\n");

    CreateAsmFile(language, tree_node->right_node, asm_file);

    free(language->array_with_variables.data);

    fprintf(asm_file, "JMP :end\n");

    fprintf(asm_file, "/ end main:\n\n");
}

void PrintDefFunction(Language* language, Tree_node* tree_node, FILE* asm_file) {
    assert(language);
    assert(tree_node);
    assert(asm_file);

    fprintf(asm_file, "/ begin def function:\n");

    Tree_node* common_node = tree_node->left_node;
    Tree_node* name_node = common_node->left_node;

    language->array_with_variables = {};
    ArrayCtor(&(language->array_with_variables), sizeof(About_variable), DEFAULT_START_CAPACITY);

    fprintf(asm_file, "\n:%s\n", NameOfVariable(name_node));

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

    for (int i = (int)language->array_with_variables.size - 1; i >= 0; --i) {
        fprintf(asm_file, "PUSH RAX\n"
                          "PUSH %d\n" 
                          "ADD\n"
                          "POP RCX\n"
                          "POP [RCX]\n", i);
    }

    fprintf(asm_file, "PUSH RAX\n"
                      "POP RBX\n"
                      "PUSH RAX\n"
                      "PUSH %d\n"
                      "ADD\n"
                      "POP RAX\n", cnt_params);

    CreateAsmFile(language, tree_node->right_node, asm_file);

    free(language->array_with_variables.data);

    fprintf(asm_file, "/ end def function\n\n");
}

void PrintCallFunction(Language* language, Tree_node* tree_node, FILE* asm_file) {
    assert(language);
    assert(tree_node);
    assert(asm_file);

    fprintf(asm_file, "/ begin call function:\n");

    Tree_node* param_node = tree_node->right_node;

    while (param_node != NULL 
           && param_node->type == OPERATOR && param_node->value.operators == OPERATOR_PARAM) {
        CreateAsmFile(language, param_node->left_node, asm_file);

        param_node = param_node->right_node;
    }

    if (param_node != NULL) {
        for (size_t i = 0; i < language->array_with_variables.size; ++i) {
            About_variable about_variable = {};
            ArrayGetElement(&language->array_with_variables, &about_variable, i);

            if (strcmp(about_variable.name, NameOfVariable(param_node)) == 0)
                fprintf(asm_file, "PUSH RBX\n"
                                  "PUSH %zu\n"
                                  "ADD\n"
                                  "POP RCX\n"
                                  "PUSH [RCX]\n", i);
        }
    }

    Tree_node* name_node = tree_node->left_node;
    fprintf(asm_file, "CALL :%s\n\n", NameOfVariable(name_node));

    fprintf(asm_file, "PUSH RAX\n"
                      "PUSH RBX\n"
                      "SUB\n"
                      "POP RCX\n"
                      "PUSH RAX\n"
                      "PUSH RCX\n"
                      "SUB\n"
                      "POP RAX\n"
                      "PUSH RBX\n"
                      "PUSH RCX\n"
                      "SUB\n"
                      "POP RBX\n");

    fprintf(asm_file, "/ end call function\n\n");
}

void PrintReturnFunction(Language* language, Tree_node* tree_node, FILE* asm_file) {
    assert(language);
    assert(tree_node);
    assert(asm_file);

    fprintf(asm_file, "/ begin return:\n");

    CreateAsmFile(language, tree_node->left_node, asm_file);

    fprintf(asm_file, "RET\n");

    fprintf(asm_file, "/ end return:\n\n");
}
