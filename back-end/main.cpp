#include <stdio.h>
#include <stdlib.h>

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
    One_label* labels_table = (One_label*)calloc(MAX_CNT_LABELS, sizeof(One_label));
    if (labels_table == NULL) return 1;

    Labels labels = {.labels_table = labels_table, .labels_count = 0, .is_second_pass = false, .current_ip = ADDR_ENTRY_POINT};
    language.cnt_labels = {0, 0, 0, 0};
    BackEndX86(&language, &labels, name_asm_file, name_bin_file);

    labels.is_second_pass = true;
    labels.current_ip = ADDR_ENTRY_POINT;
    language.cnt_labels = {0, 0, 0, 0};
    BackEndX86(&language, &labels, name_asm_file, name_bin_file);
    free(labels_table);
    #endif  

    LanguageDtor(&language);
}

/*objdump -D -b binary -m i386:x86-64 --start-address=0x2000 -M intel prog_x86.bin > dump.txt*/

/*nasm -f elf64 asm_x86.asm -o asm_x86.o*/
/*ld asm_x86.o -o asm_x86*/