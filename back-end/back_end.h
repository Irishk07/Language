#ifndef BACK_END_H_
#define BACK_END_H_

#include "../common.h"


struct Cnt_labels {
    size_t cnt_if;
    size_t cnt_while;
    size_t cnt_else;
};

struct Language {
    Tree tree;
    Array_with_data array_with_variables;
    const char* tree_file;
    char* begin_buffer;
    char* end_buffer;
    size_t size_buffer;
    Dump_information dump_info;
    Cnt_labels cnt_labels;
};

Tree_status LanguageCtor(Language* language, const char* file_with_tree,
                         const char* html_dump_filename, const char *directory);

void FillIndexesArrayWithVariables(Language* language);

Tree_status BackEnd(Language* language, const char* name_asm_file);

void CreateAsmFile(Language* language, Tree_node* tree_node, FILE* asm_file);

void PrintOperations(Language* language, Tree_node* tree_node, FILE* asm_file, const char* operation);

void PrintMathFunction(Language* language, Tree_node* tree_node, FILE* asm_file, const char* function);

void PrintAssignment(Language* language, Tree_node* tree_node, FILE* asm_file);

void PrintChange(Language* language, Tree_node* tree_node, FILE* asm_file);

void PrintIf(Language* language, Tree_node* tree_node, FILE* asm_file);

void PrintElse(Language* language, Tree_node* tree_node, FILE* asm_file);

void PrintWhile(Language* language, Tree_node* tree_node, FILE* asm_file);

void PrintOut(Language* language, Tree_node* tree_node, FILE* asm_file);

void PrintMainFunction(Language* language, Tree_node* tree_node, FILE* asm_file);

void PrintDefFunction(Language* language, Tree_node* tree_node, FILE* asm_file);

void PrintCallFunction(Language* language, Tree_node* tree_node, FILE* asm_file);

void PrintReturnFunction(Language* language, Tree_node* tree_node, FILE* asm_file);

Tree_status ReadPreOrderTreeFile(Language* language);

Tree_status ReadPreOrderNode(Language* language, Tree_node** tree_node, char** current_pos);

Tree_status LanguageDtor(Language* language);

void LanguageNodeDtor(Language* language, Tree_node* tree_node);

void ArrayDtorVariables(Array_with_data* array_with_data);


#endif // BACK_END_H_