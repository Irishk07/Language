#include <stdio.h>

#include "back_end.h"
#include "back_end_x86.h"
#include "../common.h"
#include "common_back.h"


int main(int argc, char** argv) {
    const char* file_with_tree = "../tree.txt";
    #ifdef MY_PROC
    const char* name_asm_file  = "asm/asm.txt";
    #else
    const char* name_asm_file  = "asm_x86.asm";
    const char* name_bin_file  = "prog_x86.bin";
    #endif
    const char* html_dump_filename = "dump.html";
    const char* directory = "dump";

    if (argc >= 2) file_with_tree = argv[1];
    if (argc >= 3) name_asm_file = argv[2];
    if (argc >= 4) html_dump_filename = argv[3];
    if (argc >= 5) directory = argv[4];

    Language language = {};
    LanguageCtor(&language, file_with_tree, html_dump_filename, directory);
    ReadPreOrderTreeFile(&language);

    #ifdef MY_PROC
    BackEnd(&language, name_asm_file);
    #else
    is_second_pass = false;
    current_ip = ADDR_ENTRY_POINT;
    labels_count = 0;
    language.cnt_labels = {0, 0, 0};
    BackEndX86(&language, name_asm_file, name_bin_file);

    is_second_pass = true;
    current_ip = ADDR_ENTRY_POINT;
    language.cnt_labels = {0, 0, 0};
    BackEndX86(&language, name_asm_file, name_bin_file);
    #endif  

    LanguageDtor(&language);
}

/*objdump -D -b binary -m i386:x86-64 --start-address=0x2000 -M intel prog_x86.bin > dump.txt*/