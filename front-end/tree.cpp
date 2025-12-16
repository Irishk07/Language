#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "tree.h"

#include "array.h"
#include "../common.h"
#include "front_end.h"


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

char* ReadAnswer() {
    char* answer = NULL;
    size_t size_buf = 0;

    if (getline(&answer, &size_buf, stdin) == -1)
        return NULL;

    answer[size_buf - 1] = '\0'; 

    return answer;
}

size_t IndexOfVariable(Tree_node* tree_node) {
    assert(tree_node);

    return (size_t)tree_node->value.index_variable;
}

int ValueOfVariable(Language* language, Tree_node* tree_node) {
    assert(language);
    assert(tree_node);

    return ValueOfVariableFromIndex(language, IndexOfVariable(tree_node));
}

int ValueOfVariableFromIndex(Language* language, size_t index) {
    assert(language);

    About_variable about_variable = {};
    ArrayGetElement(&(language->array_with_variables), &about_variable, index);

    return about_variable.value;
}

char* NameOfVariable(Language* language, Tree_node* tree_node) {
    assert(language);
    assert(tree_node);

    return NameOfVariableFromIndex(language, IndexOfVariable(tree_node));
}

char* NameOfVariableFromIndex(Language* language, size_t index) {
    assert(language);

    About_variable about_variable = {};
    ArrayGetElement(&(language->array_with_variables), &about_variable, index);

    return about_variable.name;
}

const char* IndetifySign(Tree_node* tree_node) {
    assert(tree_node);

    switch(tree_node->type) {
        case OPERATOR:
            switch (tree_node->value.operators) {
                case OPERATOR_ADD:           return "+";
                case OPERATOR_SUB:           return "-";
                case OPERATOR_MUL:           return "*";
                case OPERATOR_DIV:           return "/";
                case OPERATOR_POW:           return "^";
                case OPERATOR_LN:            return "ln";
                case OPERATOR_LOG:           return "log";
                case OPERATOR_SIN:           return "sin";
                case OPERATOR_COS:           return "cos";
                case OPERATOR_TG:            return "tg";
                case OPERATOR_CTG:           return "ctg";
                case OPERATOR_ARCSIN:        return "arcsin";
                case OPERATOR_ARCCOS:        return "arccos";
                case OPERATOR_ARCTG:         return "arctg";
                case OPERATOR_ARCCTG:        return "arcctg";
                case OPERATOR_SH:            return "sh";
                case OPERATOR_CH:            return "ch";
                case OPERATOR_TH:            return "th";
                case OPERATOR_CTH:           return "cth";
                case OPERATOR_ASSIGNMENT:    return ":=";
                case OPERATOR_CHANGE:        return "=";
                case OPERATOR_COMMON:        return ";";
                case OPERATOR_IF:            return "if";
                case OPERATOR_WHILE:         return "while";
                case OPERATOR_ELSE:          return "else";
                case OPERATOR_INPUT:         return "input";
                case OPERATOR_PRINT:         return "print";
                case OPERATOR_ABOVE:         return ">";
                case OPERATOR_BEFORE:        return "<";
                case OPERATOR_EQUAL:         return "==";
                case OPERATOR_OPEN_BRACKET:  return "(";
                case OPERATOR_CLOSE_BRACKET: return ")";
                case OPERATOR_OPEN_FIGURE:   return "{";
                case OPERATOR_CLOSE_FIGURE:  return "}";
                case OPERATOR_FINISH_SYMBOL: return "$";
                case OPERATOR_MATCH:         return "->";
                case OPERATOR_DEF_FUNCTION:  return "func";
                case OPERATOR_CALL_FUNCTION: return "call";
                case OPERATOR_RETURN:        return "return";
                case OPERATOR_PARAM:         return ",";
                case WRONG_OPERATOR:
                default: break;
            }
        case NUMBER:
        case VARIABLE:
        case WRONG_TYPE:
        default: break;
    }

    return NULL;
}

void SkipSpaces(char** buffer) {
    assert(buffer);
    assert(*buffer);

    while (isspace(**buffer) && (**buffer) != '\0')
        (*buffer)++;
}

void SkipComments(char** buffer) {
    assert(buffer);
    assert(*buffer);

    if (**buffer != '#')
        return;
    
    (*buffer)++;

    while (**buffer != '\n' && (**buffer) != '\0')
        (*buffer)++;

    (*buffer)++;
}

unsigned long hash_djb2(const char *str) {
    assert(str);

    unsigned long hash = 5381;
    int c = 0;

    while ((c = *(str++)) != '\0') {
        hash = ((hash << 5) + hash) + (unsigned long)c; /* hash * 33 + c */
    }

    return hash;
}


Tree_status TreeHTMLDump(Language* language, Tree_node* tree_node, int line, const char* file, Type_dump type_dump, Tree_status tree_status) {
    assert(language);
    assert(file);

    FILE* html_dump_file = NULL;
    if (language->dump_info.num_html_dump == 0)
        html_dump_file = fopen(language->dump_info.html_dump_filename, "w");
    else
        html_dump_file = fopen(language->dump_info.html_dump_filename, "a");
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

    fprintf(html_dump_file, "Root: %p\n", language->tree.root);

    TREE_CHECK_AND_RETURN_ERRORS(GenerateGraph(language, tree_node));

    char command[MAX_LEN_NAME] = {};
    snprintf(command, MAX_LEN_NAME, "dot %s/graphes/graph%d.txt -T png -o %s/images/image%d.png", 
                                     language->dump_info.directory, language->dump_info.num_html_dump, 
                                     language->dump_info.directory, language->dump_info.num_html_dump);
    
    if (system((const char*)command) != 0)
        TREE_CHECK_AND_RETURN_ERRORS(EXECUTION_FAILED,      fprintf(html_dump_file, "Error with create image:(\n"));

    fprintf(html_dump_file, "\n");
    fprintf(html_dump_file, "<img src = %s/images/image%d.png width = 1000px>", language->dump_info.directory, language->dump_info.num_html_dump);

    fprintf(html_dump_file, "\n\n");

    language->dump_info.num_html_dump++;

    if (fclose(html_dump_file) == EOF)
        TREE_CHECK_AND_RETURN_ERRORS(CLOSE_ERROR,      perror("Error is: "));

    return SUCCESS;
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
        TREE_CHECK_AND_RETURN_ERRORS(GenerateGraph(language, tree_node));

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

Tree_status GenerateGraph(Language* language, Tree_node* tree_node) {
    assert(language);

    char filename_graph[MAX_LEN_NAME] = {};
    snprintf(filename_graph, MAX_LEN_NAME, "%s/graphes/graph%d.txt", language->dump_info.directory, language->dump_info.num_html_dump);

    FILE* graph = fopen(filename_graph, "w");
    if (graph == NULL)
        TREE_CHECK_AND_RETURN_ERRORS(OPEN_ERROR);

    fprintf(graph, "digraph {\n");
    fprintf(graph, "    node [shape = Mrecord, style = filled, fillcolor = \"#99FF99\"];\n");

    if (tree_node != NULL) {
        PrintNodeToDot(language, graph, tree_node);
    } 
    else {
        fprintf(graph, "There will be no tree\n");
    }

    fprintf(graph, "}\n");

    if (fclose(graph) == EOF)
        TREE_CHECK_AND_RETURN_ERRORS(CLOSE_ERROR,    perror("Error is:"));

    return SUCCESS;
}

void PrintNodeToDot(Language* language, FILE *file, Tree_node* tree_node) {
    assert(language);
    assert(file);

    if (tree_node->type == NUMBER) {
        fprintf(file, "    node_%p [label=\"{'%d'}\", fillcolor = \"#99FF99\"];\n", 
                      (void *)tree_node, tree_node->value.number);
    }    

    else if (tree_node->type == VARIABLE) {
        fprintf(file, "    node_%p [label=\"{'%s' (%d)}\", fillcolor = \"#99c0ffff\"];\n", 
                      (void *)tree_node, NameOfVariable(language, tree_node), ValueOfVariable(language, tree_node));
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
        PrintNodeToDot(language, file, tree_node->left_node);
    }

    if (tree_node->right_node) {
        fprintf(file, "    node_%p -> node_%p [color = \"#0000CC\"];\n", (void *)tree_node, (void *)tree_node->right_node);
        PrintNodeToDot(language, file, tree_node->right_node);
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