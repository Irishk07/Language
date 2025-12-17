#ifndef TREE_H_
#define TREE_H_

#include "common.h"

#define DUMP_INFO __LINE__, __FILE__
#define NOT_ERROR_DUMP USUAL_DUMP, SUCCESS

#define DUMP_CURRENT_SITUATION(node)                              \
        TreeHTMLDump(&language->dump_info, &language->array_with_variables, node, DUMP_INFO, NOT_ERROR_DUMP);


const int MAX_LEN_NAME = 256;
const int LEN_NIL = sizeof("nil") - 1;


enum Type_dump {
    USUAL_DUMP = 0,
    ERROR_DUMP = 1
};


Tree_status TreeCtor(Tree* tree, const char* html_dump_filename, const char* directory);

Tree_node* NodeCtor(Type_node type, type_t value,
                    Tree_node* left_node, Tree_node* right_node);

size_t TreeSize(Tree_node* tree_node);

Tree_status TreeHTMLDump(Dump_information* dump_info, Array_with_data* array_with_data, Tree_node* tree_node, 
                         int line, const char* file, Type_dump type_dump, Tree_status tree_status);

Tree_status GenerateGraph(Dump_information* dump_info, Array_with_data* array_with_data, Tree_node* tree_node);

void PrintNodeToDot(Array_with_data* array_with_data, FILE *file, Tree_node* tree_node);

void PrintErrors(int error, FILE* stream);


#endif // TREE_H_