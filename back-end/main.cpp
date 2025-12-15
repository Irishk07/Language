#include <stdio.h>

#include "back_end.h"
#include "../common.h"


int main(int argc, char** argv) {
    const char* file_with_tree = "../tree.txt";
    const char* name_asm_file  = "asm/asm.txt";
    const char* html_dump_filename = "dump.html";
    const char* directory = "dump";

    if (argc >= 2)
        file_with_tree = argv[1];
    if (argc >= 3)
        name_asm_file = argv[2];
    if (argc >= 4)
        html_dump_filename = argv[3];
    if (argc >= 5)
        directory = argv[4];

    Language language = {};
    LanguageCtor(&language, file_with_tree, html_dump_filename, directory);

    BackEnd(&language, name_asm_file);

    LanguageDtor(&language);
}