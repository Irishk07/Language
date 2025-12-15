#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "onegin.h"


int OneginTextSize(const char *text_name) {
    assert(text_name);

    struct stat text_info = {};

    if (stat(text_name, &text_info) == -1) {
        perror("Error is");

        return -1;
    }

    return (int)text_info.st_size;
}

processor_status OneginReadFile(Processor* processor) {
    assert(processor);
    assert(processor->about_text.text_name);

    FILE *text = fopen(processor->about_text.text_name, "r");
    if (text == NULL) {
        CHECK_AND_RETURN_ERRORS_PROC(PROC_OPEN_ERROR,   perror("Error is"),
                                                        fclose(text));
    }

    processor->about_text.text_size = OneginTextSize(processor->about_text.text_name);
    if (processor->about_text.text_size == -1) {
        CHECK_AND_RETURN_ERRORS_PROC(PROC_STAT_ERROR,     fclose(text));
    }

    processor->about_text.text = (char*)calloc((size_t)(processor->about_text.text_size + 1), sizeof(char));
    if (processor->about_text.text == NULL) {
        CHECK_AND_RETURN_ERRORS_PROC(PROC_NOT_ENOUGH_MEMORY,    perror("Error is"),
                                                                fclose(text));
    }

    fread((char*)processor->about_text.text, sizeof(char), (size_t)processor->about_text.text_size, text);
    if (ferror(text) != 0) {
        CHECK_AND_RETURN_ERRORS_PROC(PROC_READ_ERROR,   fprintf(stderr, "Error is: problem with reading file"),
                                                        free(processor->about_text.text),
                                                        fclose(text));
    }

    char* temp_text = processor->about_text.text;

    for ( ; *temp_text != '\0'; ++temp_text) {
        if (*temp_text == '\n' || *temp_text == ' ') {
            processor->about_text.cnt_strok++;
        }
    }

    processor->about_text.cnt_strok++;

    if (fclose(text) == EOF) {
        CHECK_AND_RETURN_ERRORS_PROC(PROC_CLOSE_ERROR,    perror("Error is:"),
                                                          free(processor->about_text.text)); 
    }

    return PROC_SUCCESS;
}

processor_status DivisionIntoCommands(Processor* processor) {
    assert(processor);
    assert(processor->about_text.text);
    assert(processor->about_text.text_name);

    processor->about_text.pointer_on_text = (char**)calloc((size_t)(processor->about_text.cnt_strok + 1), sizeof(char*));
    if (processor->about_text.pointer_on_text == NULL) {
        CHECK_AND_RETURN_ERRORS_PROC(PROC_NOT_ENOUGH_MEMORY);
    }

    char* temp_text = processor->about_text.text;

    sscanf(temp_text, "%zu", &processor->cnt_commands);

    for (int i = 0; i < processor->about_text.cnt_strok && *temp_text != '\0'; ) {
        if (*temp_text == '\n' || *temp_text == ' ') {

            processor->about_text.pointer_on_text[i] = temp_text + 1;

            ++i;
        }

        temp_text++;
    }

    return PROC_SUCCESS;
}