#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "back_end.h"

#include "array.h"
#include "../common.h"
#include "tree.h"


Tree_status LanguageCtor(Language* language, const char* file_with_tree,
                         const char* html_dump_filename, const char *directory) {
    assert(language);
    assert(file_with_tree);

    language->begin_buffer = NULL;
    language->end_buffer   = NULL;
    language->size_buffer  = 0;

    language->tree_file = file_with_tree;

    language->dump_info.html_dump_filename = html_dump_filename;
    language->dump_info.directory          = directory;

    language->cnt_labels.cnt_if    = 0;
    language->cnt_labels.cnt_while = 0;
    language->cnt_labels.cnt_else  = 0;

    language->array_with_variables = {};
    ArrayCtor(&(language->array_with_variables), sizeof(About_variable), DEFAULT_START_CAPACITY);

    return SUCCESS;
}

void FillIndexesArrayWithVariables(Language* language) {
    assert(language);

    for (size_t i = 0; i < language->array_with_variables.size; ++i) {
        About_variable about_variable = {.name = NameOfVariableFromIndex(language, i), .value = (int)(i + 1)};

        ArrayChangeElement(&(language->array_with_variables), &about_variable, i);
    }
}

Tree_status BackEnd(Language* language, const char* name_asm_file) {
    assert(language);

    ReadPreOrderTreeFile(language);
    FillIndexesArrayWithVariables(language);

    FILE* asm_file = fopen(name_asm_file, "w");
    if (asm_file == NULL)
        TREE_CHECK_AND_RETURN_ERRORS(OPEN_ERROR);

    CreateAsmFile(language, language->tree.root, asm_file);

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
        fprintf(asm_file, "PUSH [%d]\n", ValueOfVariable(language, tree_node));

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
            case OPERATOR_EQUAL:  
            case OPERATOR_CHANGE: PrintAssignment(language, tree_node, asm_file);             break;
            case OPERATOR_IF:     PrintIf(language, tree_node, asm_file);                     break;
            case OPERATOR_ELSE:   break;
            case OPERATOR_WHILE:  PrintWhile(language, tree_node, asm_file);                  break;
            case OPERATOR_COMMON: CreateAsmFile(language, tree_node->left_node, asm_file);
                                  CreateAsmFile(language, tree_node->right_node, asm_file);   break;
            case OPERATOR_OPEN_BRACKET:  
            case OPERATOR_CLOSE_BRACKET: 
            case OPERATOR_OPEN_FIGURE:   
            case OPERATOR_CLOSE_FIGURE:  
            case OPERATOR_FINISH_SYMBOL:
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

    CreateAsmFile(language, tree_node->left_node, asm_file);
    CreateAsmFile(language, tree_node->right_node, asm_file);
    fprintf(asm_file, "%s\n", operation);
}

void PrintMathFunction(Language* language, Tree_node* tree_node, FILE* asm_file, const char* function) {
    assert(language);
    assert(asm_file);
    assert(tree_node);
    assert(function);

    CreateAsmFile(language, tree_node->left_node, asm_file);
    fprintf(asm_file, "%s\n", function);
}

void PrintAssignment(Language* language, Tree_node* tree_node, FILE* asm_file) {
    assert(language);
    assert(asm_file);
    assert(tree_node);

    CreateAsmFile(language, tree_node->right_node, asm_file);
    fprintf(asm_file, "POP [%d]\n", ValueOfVariable(language, tree_node->left_node));
}

void PrintIf(Language* language, Tree_node* tree_node, FILE* asm_file) {
    assert(language);
    assert(asm_file);
    assert(tree_node);

    CreateAsmFile(language, tree_node->left_node, asm_file);
    
    if (tree_node->right_node->type != OPERATOR || tree_node->right_node->value.operators != OPERATOR_ELSE) {
        fprintf(asm_file, "JE :end_if_%zu\n", language->cnt_labels.cnt_if);
        CreateAsmFile(language, tree_node->right_node, asm_file);
    }    
    else
        PrintElse(language, tree_node->right_node, asm_file);

    fprintf(asm_file, ":end_if_%zu\n", language->cnt_labels.cnt_if);

    language->cnt_labels.cnt_if++;
}

void PrintElse(Language* language, Tree_node* tree_node, FILE* asm_file) {
    assert(language);
    assert(asm_file);
    assert(tree_node);

    fprintf(asm_file, "JE :else_%zu\n", language->cnt_labels.cnt_else);

    CreateAsmFile(language, tree_node->left_node, asm_file); // in if
    fprintf(asm_file, "JMP :end_if_%zu\n", language->cnt_labels.cnt_if);
    fprintf(asm_file, ":else_%zu\n", language->cnt_labels.cnt_else);
    CreateAsmFile(language, tree_node->right_node, asm_file); // in else

    language->cnt_labels.cnt_else++;
}

void PrintWhile(Language* language, Tree_node* tree_node, FILE* asm_file) {
    assert(language);
    assert(asm_file);
    assert(tree_node);

    fprintf(asm_file, ":begin_while_%zu\n", language->cnt_labels.cnt_while);

    CreateAsmFile(language, tree_node->left_node, asm_file);

    fprintf(asm_file, "JE :end_while_%zu\n", language->cnt_labels.cnt_while);

    CreateAsmFile(language, tree_node->right_node, asm_file);
    fprintf(asm_file, "JMP :begin_while_%zu\n", language->cnt_labels.cnt_while);

    fprintf(asm_file, ":end_while_%zu\n", language->cnt_labels.cnt_while);

    language->cnt_labels.cnt_while++;
}

Tree_status LanguageDtor(Language* language) {
    ArrayDtorVariables(language, &language->array_with_variables);
    free(language->array_with_variables.data);

    LanguageNodeDtor(language, language->tree.root);

    return SUCCESS;
}

void LanguageNodeDtor(Language* language, Tree_node* tree_node) {
    if (tree_node == NULL)
        return;

    LanguageNodeDtor(language, tree_node->left_node);
    LanguageNodeDtor(language, tree_node->right_node);

    tree_node->left_node = NULL;
    tree_node->right_node = NULL;

    free(tree_node);
}

void ArrayDtorVariables(Language* language, Array_with_data* array_with_data) {
    for (size_t i = 0; i < array_with_data->size; ++i)
        free(NameOfVariableFromIndex(language, i));
}