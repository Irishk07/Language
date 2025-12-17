#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "front_end.h"

#include "../array.h"
#include "../common.h"
#include "onegin.h"
#include "../tree.h"


Tree_status LanguageCtor(Language* language, const char* html_dump_filename, const char *directory,
                                             const char* programm_file, const char* tree_file) {
    assert(language);
    assert(html_dump_filename);
    assert(directory);

    language->begin_buffer = NULL;
    language->end_buffer   = NULL;
    language->size_buffer  = 0;

    language->programm_file = programm_file;
    language->tree_file     = tree_file;

    language->dump_info.html_dump_filename = html_dump_filename;
    language->dump_info.directory          = directory;

    language->array_with_variables = {};
    ArrayCtor(&(language->array_with_variables), sizeof(About_variable), DEFAULT_START_CAPACITY);
    language->array_with_tokens = {};
    ArrayCtor(&(language->array_with_tokens), sizeof(Tree_node*), DEFAULT_START_CAPACITY);

    return SUCCESS;
}

Tree_status CreatePreOrderTreeFile(Language* language) {
    assert(language);

    FILE* tree_file = fopen(language->tree_file, "w");
    if (tree_file == NULL)
        TREE_CHECK_AND_RETURN_ERRORS(OPEN_ERROR);

    PrintPreOrderTreeToFile(language, language->tree.root, tree_file);

    if (fclose(tree_file) == EOF)
        TREE_CHECK_AND_RETURN_ERRORS(CLOSE_ERROR);

    return SUCCESS;
}

void PrintPreOrderTreeToFile(Language* language, Tree_node* tree_node, FILE* stream) {
    assert(stream);

    if (tree_node == NULL) {
        fprintf(stream, "nil ");
        return;
    }

    if (tree_node->type == NUMBER)
        fprintf(stream, "( %d ", tree_node->value.number);
    
    else if (tree_node->type == VARIABLE)
        fprintf(stream, "( \"%s\" ", NameOfVariable(tree_node));

    if (tree_node->type == OPERATOR)
        fprintf(stream, "( %s ", IndetifySign(tree_node));

    PrintPreOrderTreeToFile(language, tree_node->left_node, stream);

    PrintPreOrderTreeToFile(language, tree_node->right_node, stream);
    fprintf(stream, ") ");
}

Tree_status LanguageDtor(Language* language) {
    free(language->array_with_tokens.data);

    LanguageNodeDtor(language, language->tree.root);
    free(language->array_with_variables.data);

    return SUCCESS;
}

void LanguageNodeDtor(Language* language, Tree_node* tree_node) {
    if (tree_node == NULL)
        return;

    LanguageNodeDtor(language, tree_node->left_node);
    LanguageNodeDtor(language, tree_node->right_node);

    if (tree_node->type == VARIABLE) {
        free((void*)NameOfVariable(tree_node));
        free(tree_node->value.about_variable);
    }

    tree_node->left_node = NULL;
    tree_node->right_node = NULL;

    free(tree_node);
}

Tree_status TreeHTMLDumpArrayTokens(Language* language, size_t number_token, int line, const char* file) {
    assert(language);
    assert(file);

    FILE* html_dump_file = NULL;
    if (language->dump_info.num_html_dump == 0)
        html_dump_file = fopen(language->dump_info.html_dump_filename, "w");
    else
        html_dump_file = fopen(language->dump_info.html_dump_filename, "a");

    fprintf(html_dump_file, "(%s: %d)\n", file, line);

    for (size_t i = number_token; i < language->array_with_tokens.size; ++i) {
        Tree_node* tree_node = NULL;
        ArrayGetElement(&(language->array_with_tokens), &tree_node, i);
        TREE_CHECK_AND_RETURN_ERRORS(GenerateGraph(&language->dump_info, &language->array_with_tokens, tree_node));

        char command[MAX_LEN_NAME] = {};
        snprintf(command, MAX_LEN_NAME, "dot %s/graphes/graph%d.txt -T png -o %s/images/image%d.png", 
                                        language->dump_info.directory, language->dump_info.num_html_dump, 
                                        language->dump_info.directory, language->dump_info.num_html_dump);

        if (system((const char*)command) != 0)
            TREE_CHECK_AND_RETURN_ERRORS(EXECUTION_FAILED,      fprintf(html_dump_file, "Error with create image:(\n"));

        fprintf(html_dump_file, "\n");
        fprintf(html_dump_file, "<img src = %s/images/image%d.png width = 1000px>", language->dump_info.directory, language->dump_info.num_html_dump);

        fprintf(html_dump_file, "\n\n");

    }

    language->dump_info.num_html_dump++;

    return SUCCESS;
}