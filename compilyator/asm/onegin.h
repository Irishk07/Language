#ifndef ONEGIN_H_
#define ONEGIN_H_

#include <stdio.h>

#include "asm.h"


int OneginTextSize(const char *text_name);

assembler_status OneginReadFile(Assembler* assembler);

assembler_status DivisionIntoCommands(Assembler* assembler);


#endif // ONEGIN_H_