#ifndef COMMON_BACK_H_
#define COMMON_BACK_H_

#include <stdio.h>

#include "../common.h"


struct Cnt_labels {
    size_t cnt_if;
    size_t cnt_while;
    size_t cnt_else;
    size_t cnt_pows;
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

bool IsConstantNode(Language* language, Tree_node* node);

Tree_status ReadPreOrderTreeFile(Language* language);

Tree_status ReadPreOrderNode(Language* language, Tree_node** tree_node, char** current_pos);

Tree_status LanguageDtor(Language* language);

void LanguageNodeDtor(Language* language, Tree_node* tree_node);

void ArrayDtorVariables(Array_with_data* array_with_data);


#endif // COMMON_BACK_H_