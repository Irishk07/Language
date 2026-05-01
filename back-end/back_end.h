#ifndef BACK_END_H_
#define BACK_END_H_

#include "../common.h"
#include "common_back.h"


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


#endif // BACK_END_H_