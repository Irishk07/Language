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


Tree_status LanguageCtor(Language* language, const char* html_dump_filename, const char *directory, const char* tree_file);

Tree_status ReverseEnd(Language* language, const char* programm_file);

void CreateProgrammFile(Language* language, Tree_node* tree_node, FILE* prog_file, int* cnt_tabs);

Tree_status ReadPreOrderTreeFile(Language* language);

Tree_status ReadPreOrderNode(Language* language, Tree_node** tree_node, char** current_pos);

Tree_status LanguageDtor(Language* language);

void LanguageNodeDtor(Language* language, Tree_node* tree_node);


#endif // LANGUAGE_H_