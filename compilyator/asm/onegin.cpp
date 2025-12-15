#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "onegin.h"

#include "asm.h"


assembler_status OneginReadFile(Assembler* assembler) {
    assert(assembler);
    assert(assembler->about_text.text_name);
    

    FILE *text = fopen(assembler->about_text.text_name, "r");
    if (text == NULL) {
        CHECK_AND_RETURN_ERRORS_ASM(ASM_OPEN_ERROR,     perror("Error is"));
    }

    assembler->about_text.text_size = OneginTextSize(assembler->about_text.text_name);
    if (assembler->about_text.text_size == -1) {
        CHECK_AND_RETURN_ERRORS_ASM(ASM_STAT_ERROR,     fclose(text));
    }

    assembler->about_text.text = (char*)calloc((size_t)(assembler->about_text.text_size + 1), sizeof(char));
    if (assembler->about_text.text == NULL) {
        CHECK_AND_RETURN_ERRORS_ASM(ASM_NOT_ENOUGH_MEMORY,      perror("Error is"),
                                                                fclose(text));
    }

    fread((char*)assembler->about_text.text, sizeof(char), (size_t)assembler->about_text.text_size, text);
    if (ferror(text) != 0) {
        CHECK_AND_RETURN_ERRORS_ASM(ASM_READ_ERROR,     fprintf(stderr, "Error is: problem with reading file"),
                                                        free(assembler->about_text.text),
                                                        fclose(text));
    }

    char* temp_text = assembler->about_text.text;

    for ( ; *temp_text != '\0'; ++temp_text) {
        if (*temp_text == '/') {
            while(*temp_text != '\n') temp_text++;
        }
    
        if (*temp_text == '\n' || *temp_text == ' ') {
            assembler->about_text.cnt_strok++;
        }
    }

    assembler->about_text.cnt_strok++;

    if (fclose(text) == EOF) {
        CHECK_AND_RETURN_ERRORS_ASM(ASM_CLOSE_ERROR,    perror("Error is:")); 
    }

    return ASM_SUCCESS;
}

int OneginTextSize(const char *text_name) {
    assert(text_name);

    struct stat text_info = {};

    if (stat(text_name, &text_info) == -1) {
        perror("Error is");

        return -1;
    }

    return (int)text_info.st_size;
}

assembler_status DivisionIntoCommands(Assembler* assembler) {
    assert(assembler);
    assert(assembler->about_text.text_name);
    assert(assembler->about_text.text);

    assembler->about_text.pointer_on_text = (char**)calloc((size_t)(assembler->about_text.cnt_strok + 1), sizeof(char*));
    if (assembler->about_text.pointer_on_text == NULL) {
        CHECK_AND_RETURN_ERRORS_ASM(ASM_NOT_ENOUGH_MEMORY);
    }

    char* temp_text = assembler->about_text.text;
    assembler->about_text.pointer_on_text[0] = temp_text;

    for (int i = 1; i < assembler->about_text.cnt_strok && *temp_text != '\0'; ) {
        if (*temp_text == '/') {
            *temp_text = '\0';
            while(*temp_text != '\n') temp_text++;
        }

        if (*temp_text == '\n' || *temp_text == ' ') {
            *temp_text = '\0';

            assembler->about_text.pointer_on_text[i] = temp_text + 1;

            ++i;
        }

        temp_text++;
    }

    return ASM_SUCCESS;
}