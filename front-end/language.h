#ifndef LANGUAGE_H_
#define LANGUAGE_H_

#include "../common.h"


struct Language {
    Tree tree;
    Array_with_data array_with_variables;
    Array_with_data array_with_tokens;
    const char* programm_file;
    const char* tree_file;
    size_t size_buffer;
    char* begin_buffer;
    char* end_buffer;
    Dump_information dump_info;
};


Tree_status LanguageCtor(Language* language, const char* html_dump_filename, const char* directory,
                                             const char* programm_file, const char* tree_file);

Tree_status CreatePreOrderTreeFile(Language* language);

void PrintPreOrderTreeToFile(Language* language, Tree_node* tree_node, FILE* stream);

Tree_status LanguageDtor(Language* language);

void LanguageNodeDtor(Language* language, Tree_node* tree_node);

void ArrayDtorVariables(Language* language, Array_with_data* array_with_data);



#endif // LANGUAGE_H_