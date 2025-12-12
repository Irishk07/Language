#include <stdlib.h>

#include "asm.h"

int main(int argc, char** argv) {
    Assembler assembler = {};

    const char* name_command_file = "commands_func_new.txt";
    if (argc >= 2)
        name_command_file = argv[1];

    AsmCtor(&assembler, name_command_file);

    // first compile
    CHECK_AND_RETURN_ERRORS_ASM(Assemblirovanie(&assembler),        AsmDtor(&assembler));

    CHECK_AND_RETURN_ERRORS_ASM(CreatByteCodeData(&assembler),        AsmDtor(&assembler));

    // second compile
    CHECK_AND_RETURN_ERRORS_ASM(Assemblirovanie(&assembler),        AsmDtor(&assembler));

    CHECK_AND_RETURN_ERRORS_ASM(CreateExeFile(&assembler, "../processor/byte_code.txt"),      AsmDtor(&assembler));

    AsmDtor(&assembler);

    return 0;
}