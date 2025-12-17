#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tree.h"

#include "common.h"


Tree_status TreeCtor(Tree* tree, const char* html_dump_filename, const char* directory) {
    assert(tree);
    assert(html_dump_filename);
    assert(directory);

    tree->size = 0;

    return SUCCESS;
}

Tree_node* NodeCtor(Type_node type, type_t value,
                    Tree_node* left_node, Tree_node* right_node) {

    Tree_node* new_node = (Tree_node*)calloc(1, sizeof(Tree_node));
    if (new_node == NULL)
        return NULL;

    new_node->left_node  = left_node;
    new_node->right_node = right_node;
    new_node->type       = type;
    new_node->value      = value;

    return new_node;
}

size_t TreeSize(Tree_node* tree_node) {
    if (tree_node == NULL) {
        return 0;
    }
    
    return 1 + TreeSize(tree_node->left_node) + TreeSize(tree_node->right_node);
}

Tree_status TreeHTMLDump(Dump_information* dump_info, Array_with_data* array_with_data, Tree_node* tree_node, 
                         int line, const char* file, Type_dump type_dump, Tree_status tree_status) {
    assert(dump_info);
    assert(file);

    FILE* html_dump_file = NULL;
    if (dump_info->num_html_dump == 0)
        html_dump_file = fopen(dump_info->html_dump_filename, "w");
    else
        html_dump_file = fopen(dump_info->html_dump_filename, "a");
    if (html_dump_file == NULL)
        TREE_CHECK_AND_RETURN_ERRORS(OPEN_ERROR);

    fprintf(html_dump_file, "<pre> <font size = \"8\">\n");

    if (type_dump == USUAL_DUMP)
        fprintf(html_dump_file, "<h3> DUMP <font color = green> Tree </font> </h3>\n");
    
    if (type_dump == ERROR_DUMP) {
        fprintf(html_dump_file, "<h3> ERROR_ANSWER DUMP <font color = red> Tree </font> </h3>\n");

        PrintErrors(tree_status, html_dump_file);
    }

    fprintf(html_dump_file, "Tree {%s: %d}\n", file, line);

    TREE_CHECK_AND_RETURN_ERRORS(GenerateGraph(dump_info, array_with_data, tree_node));

    char command[MAX_LEN_NAME] = {};
    snprintf(command, MAX_LEN_NAME, "dot %s/graphes/graph%d.txt -T png -o %s/images/image%d.png", 
                                     dump_info->directory, dump_info->num_html_dump, 
                                     dump_info->directory, dump_info->num_html_dump);
    
    if (system((const char*)command) != 0)
        TREE_CHECK_AND_RETURN_ERRORS(EXECUTION_FAILED,      fprintf(html_dump_file, "Error with create image:(\n"));

    fprintf(html_dump_file, "\n");
    fprintf(html_dump_file, "<img src = %s/images/image%d.png width = 1000px>", dump_info->directory, dump_info->num_html_dump);

    fprintf(html_dump_file, "\n\n");

    dump_info->num_html_dump++;

    if (fclose(html_dump_file) == EOF)
        TREE_CHECK_AND_RETURN_ERRORS(CLOSE_ERROR,      perror("Error is: "));

    return SUCCESS;
}

Tree_status GenerateGraph(Dump_information* dump_info, Array_with_data* array_with_data, Tree_node* tree_node) {
    assert(dump_info);

    char filename_graph[MAX_LEN_NAME] = {};
    snprintf(filename_graph, MAX_LEN_NAME, "%s/graphes/graph%d.txt", dump_info->directory, dump_info->num_html_dump);

    FILE* graph = fopen(filename_graph, "w");
    if (graph == NULL)
        TREE_CHECK_AND_RETURN_ERRORS(OPEN_ERROR);

    fprintf(graph, "digraph {\n");
    fprintf(graph, "    node [shape = Mrecord, style = filled, fillcolor = \"#99FF99\"];\n");

    if (tree_node != NULL) {
        PrintNodeToDot(array_with_data, graph, tree_node);
    } 
    else {
        fprintf(graph, "There will be no tree\n");
    }

    fprintf(graph, "}\n");

    if (fclose(graph) == EOF)
        TREE_CHECK_AND_RETURN_ERRORS(CLOSE_ERROR,    perror("Error is:"));

    return SUCCESS;
}

void PrintNodeToDot(Array_with_data* array_with_data, FILE *file, Tree_node* tree_node) {
    assert(array_with_data);
    assert(file);

    if (tree_node->type == NUMBER) {
        fprintf(file, "    node_%p [label=\"{'%d'}\", fillcolor = \"#99FF99\"];\n", 
                      (void *)tree_node, tree_node->value.number);
    }    

    else if (tree_node->type == VARIABLE) {
        fprintf(file, "    node_%p [label=\"{'%s' (%d)}\", fillcolor = \"#99c0ffff\"];\n", 
                      (void *)tree_node, NameOfVariable(tree_node), ValueOfVariable(tree_node));
    }

    else if (tree_node->type != WRONG_TYPE) {
        const char* sign = IndetifySign(tree_node);
        if (*sign == '<')
            fprintf(file, "    node_%p [label=\"{'&lt;'}\", fillcolor = \"#ff99ffff\"];\n", 
                         (void *)tree_node);
        else if (*sign == '>')
            fprintf(file, "    node_%p [label=\"{'&gt;'}\", fillcolor = \"#ff99ffff\"];\n", 
                         (void *)tree_node);  
        else if (strcmp(sign, "->") == 0)
            fprintf(file, "    node_%p [label=\"{'-&gt;'}\", fillcolor = \"#ff99ffff\"];\n", 
                         (void *)tree_node); 
        else
            fprintf(file, "    node_%p [label=\"{'%s'}\", fillcolor = \"#ff99ffff\"];\n", 
                        (void *)tree_node, sign);
    }  


    if (tree_node->left_node) {
        fprintf(file, "    node_%p -> node_%p [color = \"#FF6600\"];\n ", (void *)tree_node, (void *)tree_node->left_node);
        PrintNodeToDot(array_with_data, file, tree_node->left_node);
    }

    if (tree_node->right_node) {
        fprintf(file, "    node_%p -> node_%p [color = \"#0000CC\"];\n", (void *)tree_node, (void *)tree_node->right_node);
        PrintNodeToDot(array_with_data, file, tree_node->right_node);
    }
}

void PrintErrors(int error, FILE* stream) {
    assert(stream);

    if (error == SUCCESS                  ) fprintf(stream, "ALL_RIGHT\n");
    if (error == MEMORY_ERROR             ) fprintf(stream, "Not enough memory\n");
    if (error == OPEN_ERROR               ) fprintf(stream, "Open error\n");
    if (error == CLOSE_ERROR              ) fprintf(stream, "Close error\n");
    if (error == EXECUTION_FAILED         ) fprintf(stream, "Execution failed\n");
    if (error == WRONG_SITUATION          ) fprintf(stream, "Left node is NULL, but right node not OR right node is NULL, but left node not\n");
    if (error == READ_ERROR               ) fprintf(stream, "Scanf can't read user's answers\n");
}