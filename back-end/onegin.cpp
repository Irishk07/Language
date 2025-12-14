#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "onegin.h"

#include "../common.h"
#include "back_end.h"


int SizeOfText(const char *text_name) {
    assert(text_name);

    struct stat text_info = {};

    if (stat(text_name, &text_info) == -1) {
        perror("Error is");

        return -1;
    }

    return (int)text_info.st_size;
}

Tree_status ReadOnegin(Language* language, const char* name_file) {
    assert(language);    
    assert(name_file);

    FILE *text = fopen(name_file, "r");
    if (text == NULL) {
        perror("Error is");
        return OPEN_ERROR;
    }

    int size = SizeOfText(name_file);
    if (size == -1) {
        return STAT_ERROR;
    }
    language->size_buffer = (size_t)size;

    language->begin_buffer = (char*)calloc((language->size_buffer + 1), sizeof(char));
    if (language->begin_buffer == NULL)
        TREE_CHECK_AND_RETURN_ERRORS(MEMORY_ERROR);

    language->end_buffer = language->begin_buffer + language->size_buffer;

    fread((char *)language->begin_buffer, sizeof(char), language->size_buffer, text);
    if (ferror(text) != 0) {
        TREE_CHECK_AND_RETURN_ERRORS(READ_ERROR,    free(language->begin_buffer));
    }

    // *(*str + size) = '\0';

    return SUCCESS;
}