#ifndef LANGUAGE_H_
#define LANGUAGE_H_

#include "common.h"


Tree_status LanguageCtor(Language* language, const char* html_dump_filename, const char* directory);

Tree_status CreatePreOrderTreeFile(Language* language, const char* programm_file);

void PrintPreOrderTreeToFile(Language* language, Tree_node* tree_node, FILE* stream);

Tree_status Middle_end(Language* language, const char* file_with_tree);

Tree_status ReadPreOrderTreeFile(Language* language, const char* file_with_tree);

Tree_status ReadPreOrderNode(Language* language, Tree_node** tree_node, char** current_pos);

Type_node FindOutType(Language* language, const char* name, type_t* value);

Status_of_finding ItIsOperator(const char* name, type_t* value);

Status_of_finding ItIsNumber(const char* name, type_t* value);

Status_of_finding ItIsVariable(Language* language, const char* name, type_t* value);

Tree_status LanguageDtor(Language* language);

void LanguageNodeDtor(Language* language, Tree_node* tree_node);

void ArrayDtorVariables(Language* language, Array_with_data* array_with_data);



#endif // LANGUAGE_H_