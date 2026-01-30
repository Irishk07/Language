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

    FILE* prog_file = fopen(programm_file, "w");
    if (prog_file == NULL)
        TREE_CHECK_AND_RETURN_ERRORS(OPEN_ERROR);

    TreeHTMLDump(&language->dump_info, &language->array_with_variables, language->tree.root, DUMP_INFO, NOT_ERROR_DUMP);

    int cnt_tabs = 0;
    CreateProgrammFile(language, language->tree.root, prog_file, &cnt_tabs);

    fprintf(prog_file, "\n\n%s\n", key_words[OPERATOR_FINISH_SYMBOL].name);

    if (fclose(prog_file) == EOF)
        TREE_CHECK_AND_RETURN_ERRORS(CLOSE_ERROR);

    return SUCCESS;
}

void CreateProgrammFile(Language* language, Tree_node* tree_node, FILE* prog_file, int* cnt_tabs) {
    assert(language);
    assert(prog_file);

    if (tree_node == NULL)
        return;

    if (tree_node->type == NUMBER)
        fprintf(prog_file, "%d", tree_node->value.number);
    if (tree_node->type == VARIABLE)
        fprintf(prog_file, "%s", NameOfVariable(tree_node));              

    if (tree_node->type == OPERATOR) {
        if (tree_node->value.operators == OPERATOR_COMMON) {
            CreateProgrammFile(language, tree_node->left_node, prog_file, cnt_tabs);
            CreateProgrammFile(language, tree_node->right_node, prog_file, cnt_tabs);
        }    
        else
            key_words[tree_node->value.operators].function(language, tree_node, prog_file, cnt_tabs);
    }
}

#define TABS                      \
    int temp_tabs = *cnt_tabs;    \
    while (temp_tabs-- > 0)       \
        fprintf(prog_file, "\t");

#define TABS_REPEAT               \
    temp_tabs = *cnt_tabs;        \
    while (temp_tabs-- > 0)       \
        fprintf(prog_file, "\t");

void do_math_operations(Language* language, Tree_node* tree_node, FILE* prog_file, int* cnt_tabs) {
    assert(language);
    assert(tree_node);
    assert(prog_file);

    CreateProgrammFile(language, tree_node->left_node, prog_file, cnt_tabs);
    fprintf(prog_file, " %s ", key_words[tree_node->value.operators].name);
    CreateProgrammFile(language, tree_node->right_node, prog_file, cnt_tabs);
}

void do_math_functions(Language* language, Tree_node* tree_node, FILE* prog_file, int* cnt_tabs) {
    assert(language);
    assert(tree_node);
    assert(prog_file);

    fprintf(prog_file, "%s *_^ ", key_words[tree_node->value.operators].name);
    CreateProgrammFile(language, tree_node->left_node, prog_file, cnt_tabs);
    fprintf(prog_file, " ^_*");
}

void do_if(Language* language, Tree_node* tree_node, FILE* prog_file, int* cnt_tabs) {
    assert(language);
    assert(tree_node);
    assert(prog_file);

    fprintf(prog_file, "\n");
    TABS;

    fprintf(prog_file, "%s *_^ ", key_words[tree_node->value.operators].name);
    CreateProgrammFile(language, tree_node->left_node, prog_file, cnt_tabs);
    fprintf(prog_file, " ^_* +___-\n");

    Tree_node* right_node = tree_node->right_node;
    if (right_node->type == OPERATOR && right_node->value.operators == OPERATOR_ELSE) {
        Tree_node* else_node = right_node;

        (*cnt_tabs)++;
        CreateProgrammFile(language, else_node->left_node, prog_file, cnt_tabs);
        
        (*cnt_tabs)--;
        TABS_REPEAT;
        fprintf(prog_file, "-___+\n");

        do_else(language, else_node, prog_file, cnt_tabs);
    }
    else {
        (*cnt_tabs)++;
        CreateProgrammFile(language, tree_node->right_node, prog_file, cnt_tabs);

        (*cnt_tabs)--;
        TABS_REPEAT;
        fprintf(prog_file, "-___+\n");
    }
}

void do_else(Language* language, Tree_node* tree_node, FILE* prog_file, int* cnt_tabs) {
    assert(language);
    assert(tree_node);
    assert(prog_file);

    TABS;

    fprintf(prog_file, "%s +___-\n", key_words[tree_node->value.operators].name);

    (*cnt_tabs)++;
    CreateProgrammFile(language, tree_node->right_node, prog_file, cnt_tabs);

    (*cnt_tabs)--;
    TABS_REPEAT;
    fprintf(prog_file, "-___+\n");
}

void do_while(Language* language, Tree_node* tree_node, FILE* prog_file, int* cnt_tabs) {
    assert(language);
    assert(tree_node);
    assert(prog_file);

    fprintf(prog_file, "\n");
    TABS;

    fprintf(prog_file, "%s *_^ ", key_words[tree_node->value.operators].name);
    CreateProgrammFile(language, tree_node->left_node, prog_file, cnt_tabs);
    fprintf(prog_file, " ^_* +___-\n");

    (*cnt_tabs)++;
    CreateProgrammFile(language, tree_node->right_node, prog_file, cnt_tabs);

    (*cnt_tabs)--;
    TABS_REPEAT;
    fprintf(prog_file, "-___+\n");
}

void do_print(Language* language, Tree_node* tree_node, FILE* prog_file, int* cnt_tabs) {
    assert(language);
    assert(tree_node);
    assert(prog_file);

    TABS;

    fprintf(prog_file, "%s *_^ ", key_words[tree_node->value.operators].name);
    CreateProgrammFile(language, tree_node->left_node, prog_file, cnt_tabs);
    fprintf(prog_file, " ^_* %s\n", key_words[OPERATOR_COMMON].name);
}

void do_input(Language* language, Tree_node* tree_node, FILE* prog_file, int*) {
    assert(language);
    assert(tree_node);
    assert(prog_file);

    fprintf(prog_file, "%s", key_words[tree_node->value.operators].name);
}

void do_draw(Language* language, Tree_node* tree_node, FILE* prog_file, int* cnt_tabs) {
    assert(language);
    assert(tree_node);
    assert(prog_file);

    fprintf(prog_file, "\n");
    TABS;

    fprintf(prog_file, "%s %s\n", key_words[tree_node->value.operators].name, key_words[OPERATOR_COMMON].name);
}

void do_assignment_change(Language* language, Tree_node* tree_node, FILE* prog_file, int* cnt_tabs) {
    assert(language);
    assert(tree_node);
    assert(prog_file);

    fprintf(prog_file, "\n");
    TABS;

    fprintf(prog_file, "%s ", key_words[tree_node->value.operators].name);
    CreateProgrammFile(language, tree_node->right_node, prog_file, cnt_tabs);
    fprintf(prog_file, " -> ");
    CreateProgrammFile(language, tree_node->left_node, prog_file, cnt_tabs);
    fprintf(prog_file, " %s\n", key_words[OPERATOR_COMMON].name);

    TABS_REPEAT;

    if (tree_node->value.operators == OPERATOR_ASSIGNMENT)
        fprintf(prog_file, "%s ", key_words[OPERATOR_C_ASSIGNMENT].name);
    else
        fprintf(prog_file, "%s ", key_words[OPERATOR_C_CHANGE].name);
    CreateProgrammFile(language, tree_node->right_node, prog_file, cnt_tabs);
    fprintf(prog_file, " -> ");
    CreateProgrammFile(language, tree_node->left_node, prog_file, cnt_tabs);
    fprintf(prog_file, " %s\n", key_words[OPERATOR_COMMON].name);
}

void do_def_func(Language* language, Tree_node* tree_node, FILE* prog_file, int* cnt_tabs) {
    assert(language);
    assert(tree_node);
    assert(prog_file);

    (*cnt_tabs) = 0;

    Tree_node* param_node = tree_node->left_node;
    Tree_node* func_name = param_node->left_node;
    fprintf(prog_file, "\n\n%s ", key_words[tree_node->value.operators].name);
    fprintf(prog_file, "%s *_^ ", NameOfVariable(func_name));

    while (param_node->right_node->type == OPERATOR) {
        param_node = param_node->right_node;
        fprintf(prog_file, "%s and ", NameOfVariable(param_node->left_node));
    }
    fprintf(prog_file, "%s", NameOfVariable(param_node->right_node));
    fprintf(prog_file, "^_* +___-\n");

    (*cnt_tabs)++;
    CreateProgrammFile(language, tree_node->right_node, prog_file, cnt_tabs);
    fprintf(prog_file, "-___+\n");
}

void do_main_func(Language* language, Tree_node* tree_node, FILE* prog_file, int* cnt_tabs) {
    assert(language);
    assert(tree_node);
    assert(prog_file);

    (*cnt_tabs) = 0;

    Tree_node* func_name = tree_node->left_node;
    fprintf(prog_file, "%s %s +___-\n", key_words[tree_node->value.operators].name, NameOfVariable(func_name));

    (*cnt_tabs)++;
    CreateProgrammFile(language, tree_node->right_node, prog_file, cnt_tabs);
    fprintf(prog_file, "-___+\n");
}

void do_call_func(Language* language, Tree_node* tree_node, FILE* prog_file, int*) {
    assert(language);
    assert(tree_node);
    assert(prog_file);

    Tree_node* func_name = tree_node->left_node;
    fprintf(prog_file, "%s %s *_^ ", key_words[tree_node->value.operators].name, NameOfVariable(func_name));
    
    Tree_node* param_node = tree_node->right_node;
    while (param_node->right_node->type == OPERATOR) {
        param_node = param_node->right_node;
        fprintf(prog_file, "%s and ", NameOfVariable(param_node->left_node));
    }
    fprintf(prog_file, "%s ", NameOfVariable(param_node->right_node));
    fprintf(prog_file, "^_*");
}

void do_return(Language* language, Tree_node* tree_node, FILE* prog_file, int* cnt_tabs) {
    assert(language);
    assert(tree_node);
    assert(prog_file);

    TABS;

    fprintf(prog_file, "%s ", key_words[tree_node->value.operators].name);
    CreateProgrammFile(language, tree_node->left_node, prog_file, cnt_tabs);
    fprintf(prog_file, " %s\n", key_words[OPERATOR_COMMON].name);
}


Tree_status ReadPreOrderTreeFile(Language* language) {
    assert(language);

    TREE_CHECK_AND_RETURN_ERRORS(ReadOnegin(language, language->tree_file));

    char* begin_buffer = language->begin_buffer;

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