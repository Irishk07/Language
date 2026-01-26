#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "reverse_end.h"

#include "../array.h"
#include "../common.h"
#include "onegin.h"
#include "../tree.h"


Tree_status LanguageCtor(Language* language, const char* html_dump_filename, const char *directory, const char* tree_file) {
    assert(language);
    assert(html_dump_filename);
    assert(directory);

    language->begin_buffer = NULL;
    language->end_buffer   = NULL;
    language->size_buffer  = 0;

    language->tree_file = tree_file;

    language->dump_info.html_dump_filename = html_dump_filename;
    language->dump_info.directory          = directory;

    language->array_with_variables = {};
    ArrayCtor(&(language->array_with_variables), sizeof(About_variable), DEFAULT_START_CAPACITY);

    return SUCCESS;
}

Tree_status ReverseEnd(Language* language, const char* programm_file) {
    assert(language);

    ReadPreOrderTreeFile(language);

    FILE* prog_file = fopen(programm_file, "a");
    if (prog_file == NULL)
        TREE_CHECK_AND_RETURN_ERRORS(OPEN_ERROR);

    CreateProgrammFile(language, language->tree.root, prog_file);

    if (fclose(prog_file) == EOF)
        TREE_CHECK_AND_RETURN_ERRORS(CLOSE_ERROR);

    return SUCCESS;
}

void CreateProgrammFile(Language* language, Tree_node* tree_node, FILE* prog_file) {
    assert(language);
    assert(prog_file);

    if (tree_node == NULL)
        return;

    if (tree_node->type == NUMBER)
        fprintf(prog_file, "%d ", tree_node->value.number);
    if (tree_node->type == VARIABLE)
        fprintf(prog_file, "%s ", NameOfVariable(tree_node));              

    if (tree_node->type == OPERATOR) {
        if (tree_node->value.operators == OPERATOR_CLOSE_FIGURE || tree_node->value.operators == OPERATOR_DEF_FUNCTION)
            fprintf(prog_file, "\n");

        PrintKeyWord(tree_node, prog_file);

        if (tree_node->value.operators == OPERATOR_OPEN_FIGURE || tree_node->value.operators == OPERATOR_CLOSE_FIGURE)
            fprintf(prog_file, "\n"); 
    }

    CreateProgrammFile(language, tree_node->left_node, prog_file);
    CreateProgrammFile(language, tree_node->right_node, prog_file);
}

void PrintKeyWord(Tree_node* tree_node, FILE* prog_file) {
    assert(tree_node);

    unsigned long hash = hash_djb2(NameOfVariable(tree_node));

    for (size_t i = 0; i < sizeof(key_words) / sizeof(key_words[0]); ++i) {
        if (hash == key_words[i].tree_hash && strcmp(NameOfVariable(tree_node), key_words[i].tree_name) == 0) {
            fprintf(prog_file, "%s \n", NameOfVariable(tree_node));
        }
    }
}

Tree_status ReadPreOrderTreeFile(Language* language) {
    assert(language);

    TREE_CHECK_AND_RETURN_ERRORS(ReadOnegin(language, language->tree_file));

    char* begin_buffer = language->begin_buffer;
    fprintf(stderr, "'%s'\n", begin_buffer);

    TREE_CHECK_AND_RETURN_ERRORS(ReadPreOrderNode(language, &language->tree.root, &begin_buffer));

    DUMP_CURRENT_SITUATION(language->tree.root);

    free(language->begin_buffer);
    language->begin_buffer = NULL;
    language->end_buffer   = NULL;
    language->size_buffer  = 0;

    return SUCCESS;
}

Tree_status ReadPreOrderNode(Language* language, Tree_node** tree_node, char** current_pos) {    
    assert(language);
    assert(tree_node);
    assert(current_pos);
    assert(*current_pos);

    SkipSpaces(current_pos);

    if (*current_pos > language->end_buffer)
        TREE_CHECK_AND_RETURN_ERRORS(BUFFER_OVERFLOW);

    if (**current_pos == '(') {
        (*current_pos)++;
        SkipSpaces(current_pos);

        if (strncmp(*current_pos, "nil", LEN_NIL) == 0) {
            *current_pos += LEN_NIL;
            *tree_node = NULL;

            SkipSpaces(current_pos);
        }

        else {
            if (**current_pos == '\"') // open_"
                (*current_pos)++;

            int read_bytes = 0;
            sscanf(*current_pos, "%*s%n", &read_bytes);

            if (*(*current_pos + read_bytes - 1) == '\"')
                *(*current_pos + read_bytes - 1) = '\0'; // close_" -> '\0'
            
            char* name = strndup(*current_pos, (size_t)read_bytes);
            type_t value = {.number = 0};

            Type_node type = FindOutType(name, &value);

            if (type != VARIABLE)
                free(name);

            *tree_node = NodeCtor(type, value, NULL, NULL);

            *current_pos += read_bytes;

            SkipSpaces(current_pos);

            TREE_CHECK_AND_RETURN_ERRORS(ReadPreOrderNode(language, &(*tree_node)->left_node, current_pos));

            TREE_CHECK_AND_RETURN_ERRORS(ReadPreOrderNode(language, &(*tree_node)->right_node, current_pos));

            SkipSpaces(current_pos);

            (*current_pos)++; // ++ because skip ')'

            SkipSpaces(current_pos);
        }
    }

    else if (strncmp(*current_pos, "nil", LEN_NIL) == 0) {
        *current_pos += LEN_NIL;

        *tree_node = NULL;

        SkipSpaces(current_pos);
    }

    else {
        fprintf(stderr, "'%s'\n", *current_pos);
        TREE_CHECK_AND_RETURN_ERRORS(SYNTAX_ERROR);
    }

    return SUCCESS;
}

Tree_status LanguageDtor(Language* language) {
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