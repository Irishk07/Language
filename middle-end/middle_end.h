#ifndef LANGUAGE_H_
#define LANGUAGE_H_

#include "../common.h"


struct Language {
    Tree tree;
    Array_with_data array_with_variables;
    const char* tree_file;
    char* begin_buffer;
    char* end_buffer;
    size_t size_buffer;
    Dump_information dump_info;
};


Tree_status LanguageCtor(Language* language, const char* html_dump_filename, const char* directory, const char* tree_file);

Tree_status Middle_end(Language* language);

Tree_status CreatePreOrderTreeFile(Language* language);

void PrintPreOrderTreeToFile(Language* language, Tree_node* tree_node, FILE* stream);

Tree_status ReadPreOrderTreeFile(Language* language);

Tree_status ReadPreOrderNode(Language* language, Tree_node** tree_node, char** current_pos);

Type_node FindOutType(Language* language, const char* name, type_t* value);

Status_of_finding ItIsOperator(const char* name, type_t* value);

Status_of_finding ItIsNumber(const char* name, type_t* value);

Status_of_finding ItIsVariable(Language* language, const char* name, type_t* value);

Tree_status LanguageDtor(Language* language);

void LanguageNodeDtor(Language* language, Tree_node* tree_node);

void ArrayDtorVariables(Language* language, Array_with_data* array_with_data);



#endif // LANGUAGE_H_