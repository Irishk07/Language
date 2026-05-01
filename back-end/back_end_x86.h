#ifndef BACK_END_X86_H_
#define BACK_END_X86_H_

#include "../common.h"
#include "common_back.h"


#define PUSH_NUM_(num)         fprintf(asm_file, "    push %d\n", num);
#define PUSH_R_(reg)           fprintf(asm_file, "    push " #reg "\n");
#define PUSH_M_(reg)           fprintf(asm_file, "    push qword [" #reg "]\n");

#define POP_R_(reg)            fprintf(asm_file, "    pop " #reg "\n");
#define POP_M_(reg)            fprintf(asm_file, "    pop qword [" #reg "]\n");

#define MOV_R1_R2_(reg1, reg2) fprintf(asm_file, "    mov " #reg1 ", " #reg2 "\n");
#define MOV_M_R_(reg1, reg2)   fprintf(asm_file, "    mov [" #reg1 "], " #reg2 "\n");
#define MOV_R_M_(reg1, reg2)   fprintf(asm_file, "    mov " #reg1 ", [" #reg2 "]\n");
#define MOV_R_NUM_(reg, num)   fprintf(asm_file, "    mov " #reg ", %d\n", num);
#define MOV_R_CHAR_(reg, symb) fprintf(asm_file, "    mov " #reg ", '" #symb "'\n");
#define MOV_FORMAT_IN_(reg)    fprintf(asm_file, "    mov " #reg ", format_in\n");
#define MOV_FORMAT_OUT_(reg)   fprintf(asm_file, "    mov " #reg ", format_out\n");
#define MOVQ_R_M_(xmm, addr)   fprintf(asm_file, "    movq " #xmm ", [" #addr "]\n");
#define MOVZX_R1_R2_(reg1, reg2)  fprintf(asm_file, "    movzx " #reg1 ", " #reg2 "\n");
#define MOVSXD_R1_R2_(reg1, reg2) fprintf(asm_file, "    movsxd " #reg1 ", " #reg2 "\n");

#define LEA_R_M_(reg, addr)    fprintf(asm_file, "    lea " #reg " , [" #addr "]\n");

#define SUB_R1_R2_(reg1, reg2) fprintf(asm_file, "    sub " #reg1 ", " #reg2 "\n");
#define SUB_R_NUM_(reg, num)   fprintf(asm_file, "    sub " #reg ", %d\n", num);
#define DEC_(reg)              fprintf(asm_file, "    dec " #reg "\n");

#define ADD_R1_R2_(reg1, reg2) fprintf(asm_file, "    add " #reg1 ", " #reg2 "\n");
#define ADD_R_NUM_(reg, num)   fprintf(asm_file, "    add " #reg ", %d\n", (num));
#define INC_(reg)              fprintf(asm_file, "    inc " #reg "\n");

#define IDIV_R_(reg)           fprintf(asm_file, "    idiv " #reg "\n");
#define DIV_R_(reg)            fprintf(asm_file, "    div " #reg "\n");

#define CMP_R1_R2_(reg1, reg2) fprintf(asm_file, "    cmp " #reg1 ", " #reg2 "\n");
#define CMP_R_NUM_(reg, num)   fprintf(asm_file, "    cmp " #reg ", %d\n", (num));
#define CMP_R_CHAR(reg, symb)  fprintf(asm_file, "    cmp " #reg ", " #symb "\n");
#define TEST_R1_R2_(reg1, reg2) fprintf(asm_file, "    test " #reg1 ", " #reg2 "\n");

#define AND_R1_R2_(reg1, reg2) fprintf(asm_file, "    and " #reg1 ", " #reg2 "\n");
#define AND_R_NUM_(reg, num)   fprintf(asm_file, "    and " #reg ", %d\n", num);
#define XOR_R1_R2_(reg1, reg2) fprintf(asm_file, "    xor " #reg1 ", " #reg2 "\n");
#define SHL_(reg, num)         fprintf(asm_file, "    shl " #reg ", %d\n", num);
#define NEG_(reg)              fprintf(asm_file, "    neg " #reg "\n");
#define ROL_R1_R2_(reg1, reg2) fprintf(asm_file, "    rol " #reg1 ", " #reg2 "\n");

#define CALL_(func)            fprintf(asm_file, "    call %s\n\n", func);
#define CALL_SCANF_            fprintf(asm_file, "    call scanf wrt ..plt\n");
#define CALL_PRINTF_           fprintf(asm_file, "    call my_printf\n");
#define SYSCALL_               fprintf(asm_file, "    syscall\n");
#define RET_                   fprintf(asm_file, "    ret\n");
#define LABEL_(label)          fprintf(asm_file, "." #label ":\n");
#define TO_LABEL_(label)       fprintf(asm_file, "." #label "\n");
#define FUNC_(func)            fprintf(asm_file, #func ":\n");
#define TO_FUNC_(func)         fprintf(asm_file, #func "\n");
#define TO_ADDR_(addr)         fprintf(asm_file, #addr "\n");

#define CVTSI2SD_(xmm, reg)    fprintf(asm_file, "    cvtsi2sd " #xmm ", " #reg "\n");

#define SQRTSD_(xmm1, xmm2)    fprintf(asm_file, "    sqrtsd " #xmm1 ", " #xmm2 "\n");

#define CVTTSD2SI_(reg, xmm)   fprintf(asm_file, "    cvttsd2si " #reg ", " #xmm "\n"); 

#define CQO_                   fprintf(asm_file, "    cqo\n");

#define JLE_                   fprintf(asm_file, "    jle ");
#define JL_                    fprintf(asm_file, "    jl ");
#define JGE_                   fprintf(asm_file, "    jge ");
#define JG_                    fprintf(asm_file, "    jg ");
#define JNE_                   fprintf(asm_file, "    jne ");
#define JE_                    fprintf(asm_file, "    je ");
#define JZ_                    fprintf(asm_file, "    jz ");
#define JNZ_                   fprintf(asm_file, "    jnz ");
#define JMP_                   fprintf(asm_file, "    jmp ");
#define LOOP_                  fprintf(asm_file, "    loop ");

#define SECTION_DATA_          fprintf(asm_file, "section .data\n"                                                      \
                                                 "    format_out      db \"Answer = %%d\", 10, 0\n"                     \
                                                 "    format_in       db \"%%lld\", 0\n\n"                              \
                                                 "    hex_chars       db \"0123456789ABCDEF\"\n"                        \
                                                 "    float_10        dd 10.0                     ; dword - 4 bytes\n"  \
                                                 "    float_mask_abs  dd 0x7FFFFFFF\n"                                  \
                                                 "    float_inf       db \"inf\", 0\n"                                  \
                                                 "    float_nan       db \"nan\", 0\n\n"                                \
                                                 "    jmp_table:\n"                                                     \
                                                 "        dq case_b                               ; %%b\n"              \
                                                 "        dq case_c                               ; %%c\n"              \
                                                 "        dq case_d                               ; %%d\n"              \
                                                 "        times ('f' - 'd' - 1) dq case_default   ; skip\n"             \
                                                 "        dq case_f                               ; %%f\n"              \
                                                 "        times ('o' - 'f' - 1) dq case_default   ; skip\n"             \
                                                 "        dq case_o                               ; %%o\n"              \
                                                 "        times ('s' - 'o' - 1) dq case_default   ; skip\n"             \
                                                 "        dq case_s                               ; %%s\n"              \
                                                 "        times ('x' - 's' - 1) dq case_default   ; skip\n"             \
                                                 "        dq case_x                               ; %%x\n\n");

#define SECTION_BSS_            fprintf(asm_file, "section .bss\n"                                                      \
                                                  "    BUF_CAPACITY equ 1024\n\n"                                       \
                                                  "    buf resb BUF_CAPACITY                     ; reserve 1024 bytes\n"\
                                                  "    buf_len resq 1\n"                                                \
                                                  "    xmm_save resq 8\n\n");

#define MACRO_                  fprintf(asm_file, "%%macro SAVE_IN_STACK 2\n"                                           \
                                                  "    mov [rbp - 8 * %%2], %%1\n"                                      \
                                                  "%%endmacro\n\n"                                                      \
                                                  "%%macro SAVE_FLOAT 2\n"                                              \
                                                  "    movq [xmm_save + 8 * %%2], %%1    ; mov 8 byte from xmm\n"       \
                                                  "%%endmacro\n\n");
#define SAVE_FLOAT_(xmm, n)     fprintf(asm_file, "    SAVE_FLOAT " #xmm ", %d\n", n);
#define SAVE_IN_STACK_(reg, n)  fprintf(asm_file, "    SAVE_IN_STACK " #reg ", %d\n", n);
  

Tree_status BackEndX86(Language* language, const char* name_asm_file);

void CreateAsmFileX86(Language* language, Tree_node* tree_node, FILE* asm_file);

void PrintOperationsX86(Language* language, Tree_node* tree_node, FILE* asm_file, const char* operation);

void PrintOperationDivX86(Language* language, Tree_node* tree_node, FILE* asm_file);

void PrintOperationSqrtX86(Language* language, Tree_node* tree_node, FILE* asm_file);

void PrintAssignmentX86(Language* language, Tree_node* tree_node, FILE* asm_file);

void PrintChangeX86(Language* language, Tree_node* tree_node, FILE* asm_file);

void PrintIfX86(Language* language, Tree_node* tree_node, FILE* asm_file);

void PrintElseX86(Language* language, Tree_node* tree_node, FILE* asm_file);

void PrintWhileX86(Language* language, Tree_node* tree_node, FILE* asm_file);

void PrintInX86(Language* language, Tree_node* tree_node, FILE* asm_file);

void PrintOutX86(Language* language, Tree_node* tree_node, FILE* asm_file);

void PrintMainFunctionX86(Language* language, Tree_node* tree_node, FILE* asm_file);

void PrintDefFunctionX86(Language* language, Tree_node* tree_node, FILE* asm_file);

void PrintCallFunctionX86(Language* language, Tree_node* tree_node, FILE* asm_file);

void PrintReturnFunctionX86(Language* language, Tree_node* tree_node, FILE* asm_file);

void PrintFuncMyPrintf(Language* language, FILE* asm_file);


#endif // BACK_END_X86_H_