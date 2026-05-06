#ifndef BACK_END_X86_H_
#define BACK_END_X86_H_

#include "back_end_bin.h"
#include "../common.h"
#include "common_back.h"


#define PUSH_NUM_(num)         {if (is_second_pass) fprintf(asm_file, "    push %d\n", num);                              current_ip += WritePushNum(num, bin_file);}                         
#define PUSH_R_(reg)           {if (is_second_pass) fprintf(asm_file, "    push %s\n", about_registers_64[reg]);          current_ip += WritePushR(reg, bin_file);}
#define PUSH_M_(reg)           {if (is_second_pass) fprintf(asm_file, "    push qword [%s]\n", about_registers_64[reg]);  current_ip += WritePushM(reg, bin_file);}

#define POP_R_(reg)            {if (is_second_pass) fprintf(asm_file, "    pop %s\n", about_registers_64[reg]);           current_ip += WritePopR(reg, bin_file);}
#define POP_M_(reg)            {if (is_second_pass) fprintf(asm_file, "    pop qword [%s]\n", about_registers_64[reg]);   current_ip += WritePopM(reg, bin_file);}

#define MOV_R1_R2_(reg1, reg2)      {if (is_second_pass) fprintf(asm_file, "    mov %s, %s\n", about_registers_64[reg1], about_registers_64[reg2]);             current_ip += WriteMovR1R2(reg1, reg2, bin_file);}
#define MOV_MR_R_(reg1, reg2)       {if (is_second_pass) fprintf(asm_file, "    mov [%s], %s\n", about_registers_64[reg1], about_registers_64[reg2]);           current_ip += WriteMovMrR(reg1, reg2, bin_file);}
#define MOV_MR_R8_(reg1, reg2)      {if (is_second_pass) fprintf(asm_file, "    mov [%s], %s\n", about_registers_64[reg1], about_registers_8[reg2]);            current_ip += WriteMovMrR8(reg1, reg2, bin_file);}
#define MOV_R_MR_(reg1, reg2)       {if (is_second_pass) fprintf(asm_file, "    mov %s, [%s]\n", about_registers_64[reg1], about_registers_64[reg2]);           current_ip += WriteMovRMr(reg1, reg2, bin_file);}
#define MOV_R8_MR_(reg1, reg2)      {if (is_second_pass) fprintf(asm_file, "    mov %s, [%s]\n", about_registers_8[reg1], about_registers_64[reg2]);            current_ip += WriteMovR8Mr(reg1, reg2, bin_file);}
#define MOV_R_ADDR_(reg, addr)      {if (is_second_pass) fprintf(asm_file, "    lea %s, [%s]\n", about_registers_64[reg], GetAddressName(addr));                current_ip += WriteMovRAddr(reg, addr, bin_file);}
#define MOV_R_NUM_(reg, num)        {if (is_second_pass) fprintf(asm_file, "    mov %s, %ld\n", about_registers_64[reg], (int64_t)num);                         current_ip += WriteMovRNum(reg, num, bin_file);}
#define MOV_R8_CHAR_(reg, symb)     {if (is_second_pass) fprintf(asm_file, "    mov %s, '%c'\n", about_registers_8[reg], symb);                                 current_ip += WriteMovR8Char(reg, symb, bin_file);}
#define MOV_FORMAT_IN_(reg)         {if (is_second_pass) fprintf(asm_file, "    mov " #reg ", format_in\n");                                                    current_ip += WriteMovRNum(reg, ADDR_FORMAT_IN, bin_file);}
#define MOV_FORMAT_OUT_(reg)        {if (is_second_pass) fprintf(asm_file, "    mov " #reg ", format_out\n");                                                   current_ip += WriteMovRNum(reg, ADDR_FORMAT_OUT, bin_file);}
#define MOVQ_R_M_(xmm, reg)         {if (is_second_pass) fprintf(asm_file, "    movq %s, [%s]\n", about_xmm[xmm], about_registers_64[reg]);                     current_ip += WriteMovqXmmMr(xmm, reg, bin_file);}
#define MOVZX_R1_R2_(reg1, reg2)    {if (is_second_pass) fprintf(asm_file, "    movzx %s, %s\n", about_registers_64[reg1], about_registers_8[reg2]);            current_ip += WriteMovzxR1R2(reg1, reg2, bin_file);}
#define MOVZX_R_M_(reg1, reg2)      {if (is_second_pass) fprintf(asm_file, "    movzx %s, byte [%s]\n", about_registers_64[reg1], about_registers_64[reg2]);    current_ip += WriteMovzxRMr(reg1, reg2, bin_file);}
#define MOVSXD_R1_R2_(reg1, reg2)   {if (is_second_pass) fprintf(asm_file, "    movsxd %s, %s\n", about_registers_64[reg1], about_registers_32[reg2]);          current_ip += WriteMovsxdR1R2(reg1, reg2, bin_file);}

#define SUB_R1_R2_(reg1, reg2) {if (is_second_pass) fprintf(asm_file, "    sub %s, %s\n", about_registers_64[reg1], about_registers_64[reg2]); current_ip += WriteSubR1R2(reg1, reg2, bin_file);}
#define SUB_R_NUM_(reg, num)   {if (is_second_pass) fprintf(asm_file, "    sub %s, %d\n", about_registers_64[reg], num);                       current_ip += WriteSubRNum(reg, num, bin_file);}                                         
#define DEC_(reg)              {if (is_second_pass) fprintf(asm_file, "    dec %s\n", about_registers_64[reg]);                                current_ip += WriteDecR(reg, bin_file);}

#define ADD_R1_R2_(reg1, reg2) {if (is_second_pass) fprintf(asm_file, "    add %s, %s\n", about_registers_64[reg1], about_registers_64[reg2]); current_ip += WriteAddR1R2(reg1, reg2, bin_file);}
#define ADD_R_NUM_(reg, num)   {if (is_second_pass) fprintf(asm_file, "    add %s, %d\n", about_registers_64[reg], (num));                     current_ip += WriteAddRNum(reg, num, bin_file);}
#define INC_(reg)              {if (is_second_pass) fprintf(asm_file, "    inc %s\n", about_registers_64[reg]);                                current_ip += WriteIncR(reg, bin_file);}

#define IDIV_R_(reg)           {if (is_second_pass) fprintf(asm_file, "    idiv %s\n", about_registers_64[reg]);                                    current_ip += WriteIdivR(reg, bin_file);}
#define DIV_R_(reg)            {if (is_second_pass) fprintf(asm_file, "    div %s\n", about_registers_64[reg]);                                     current_ip += WriteDivR(reg, bin_file);}
#define IMUL_R_NUM_(reg, num)  {if (is_second_pass) fprintf(asm_file, "    imul %s, %d\n", about_registers_64[reg], (num));                         current_ip += WriteImulRNum(reg, num, bin_file);}
#define IMUL_R1_R2_(reg1, reg2) { if (is_second_pass) fprintf(asm_file, "    imul %s, %s\n", about_registers_64[reg1], about_registers_64[reg2]);   current_ip += WriteImulR1R2(reg1, reg2, bin_file);}

#define CMP_R1_R2_(reg1, reg2)  {if (is_second_pass) fprintf(asm_file, "    cmp %s, %s\n", about_registers_64[reg1], about_registers_64[reg2]);  current_ip += WriteCmpR1R2(reg1, reg2, bin_file);}
#define CMP_R_NUM_(reg, num)    {if (is_second_pass) fprintf(asm_file, "    cmp %s, %d\n", about_registers_64[reg], num);                        current_ip += WriteCmpRNum(reg, num, bin_file);}
#define CMP_R8_CHAR(reg, symb)  {if (is_second_pass) {                                                          \
                                    if (symb > 31 && symb < 127)                                                \
                                        fprintf(asm_file, "    cmp %s, '%c'\n", about_registers_8[reg], symb);  \
                                    else                                                                        \
                                        fprintf(asm_file, "    cmp %s, %d\n", about_registers_8[reg], symb);    \
                                }                                                                               \
                                current_ip += WriteCmpR8Char(reg, symb, bin_file);}
#define TEST_R1_R2_(reg1, reg2) {if (is_second_pass) fprintf(asm_file, "    test %s, %s\n", about_registers_64[reg1], about_registers_64[reg2]); current_ip += WriteTestR1R2(reg1, reg2, bin_file);}

#define AND_R1_R2_(reg1, reg2) {if (is_second_pass) fprintf(asm_file, "    and %s, %s\n", about_registers_64[reg1], about_registers_64[reg2]); current_ip += WriteAndR1R2(reg1, reg2, bin_file);}
#define AND_R_NUM_(reg, num)   {if (is_second_pass) fprintf(asm_file, "    and %s, %d\n", about_registers_64[reg], num);                       current_ip += WriteAndRNum(reg, num, bin_file);}
#define XOR_R1_R2_(reg1, reg2) {if (is_second_pass) fprintf(asm_file, "    xor %s, %s\n", about_registers_64[reg1], about_registers_64[reg2]); current_ip += WriteXorR1R2(reg1, reg2, bin_file);}
#define SHL_(reg, num)         {if (is_second_pass) fprintf(asm_file, "    shl %s, %d\n", about_registers_64[reg], num);                       current_ip += WriteShlRNum(reg, num, bin_file);}
#define NEG_(reg)              {if (is_second_pass) fprintf(asm_file, "    neg %s\n", about_registers_64[reg]);                                current_ip += WriteNegR(reg, bin_file);}
#define ROL_R_CL_(reg)         {if (is_second_pass) fprintf(asm_file, "    rol %s, cl\n", about_registers_64[reg]);                            current_ip += WriteRolRCl(reg, bin_file);}

#define CVTSI2SD_(xmm, reg)    {if (is_second_pass) fprintf(asm_file, "    cvtsi2sd %s, %s\n", about_xmm[xmm], about_registers_64[reg]);    current_ip += WriteCvtsi2sd(xmm, reg, bin_file);}
#define CVTTSD2SI_(reg, xmm)   {if (is_second_pass) fprintf(asm_file, "    cvttsd2si %s, %s\n", about_registers_64[reg], about_xmm[xmm]);   current_ip += WriteCvttsd2si(reg, xmm, bin_file);}
#define SQRTSD_(xmm1, xmm2)    {if (is_second_pass) fprintf(asm_file, "    sqrtsd %s, %s\n", about_xmm[xmm1], about_xmm[xmm2]);             current_ip += WriteSqrtsd(xmm1, xmm2, bin_file);}

#define CQO_                   {if (is_second_pass) fprintf(asm_file, "    cqo\n"); current_ip += WriteCqo(bin_file);}

#define CALL_(func) {                                                                                       \
    if (is_second_pass) fprintf(asm_file, "    call %s\n", func);                                           \
    uint64_t target_addr = GetLabelAddress(func);                                                           \
    int32_t offset = (is_second_pass) ? (int32_t)((int64_t)target_addr - (int64_t)(current_ip + 5)) : 0;    \
    current_ip += WriteCall(offset, bin_file);                                                              \
}

#define SYSCALL_               {if (is_second_pass) fprintf(asm_file, "    syscall\n");  current_ip += WriteSyscall(bin_file);}
#define RET_                   {if (is_second_pass) fprintf(asm_file, "    ret\n\n");    current_ip += WriteRet(bin_file);}

#define LABEL_(name) {                                      \
    if (is_second_pass) fprintf(asm_file, "%s:\n", name);   \
    else AddLabel(name, current_ip);                        \
}
#define DOT_LABEL_(name) {                                          \
    if (is_second_pass) fprintf(asm_file, "%s:\n", DOT_L_(name));   \
    else AddLabel(DOT_L_(name), current_ip);                        \
}

#define FUNC_(name) {                                               \
    if (is_second_pass) fprintf(asm_file, "\n%s:\n", name);         \
    else AddLabel(name, current_ip);                                \
}                          


#define GEN_JCC_MACRO(name, WriteJ, label_name, target_ip) {                            \
    if (is_second_pass) fprintf(asm_file, "    " #name " %s\n", label_name);            \
    int32_t offset = (is_second_pass) ? (int32_t)(target_ip - (current_ip + 6)) : 0;    \
    current_ip += WriteJ(offset, bin_file);                                             \
}
#define JLE_(label_name, target_ip)  GEN_JCC_MACRO(jle, WriteJle, label_name, target_ip)
#define JL_(label_name, target_ip)   GEN_JCC_MACRO(jl,  WriteJl,  label_name, target_ip)
#define JGE_(label_name, target_ip)  GEN_JCC_MACRO(jge, WriteJge, label_name, target_ip)
#define JG_(label_name, target_ip)   GEN_JCC_MACRO(jg,  WriteJg,  label_name, target_ip)
#define JNE_(label_name, target_ip)  GEN_JCC_MACRO(jne, WriteJne, label_name, target_ip)
#define JE_(label_name, target_ip)   GEN_JCC_MACRO(je,  WriteJe,  label_name, target_ip)
#define JNZ_(label_name, target_ip)   GEN_JCC_MACRO(jnz,  WriteJnz,  label_name, target_ip)
#define JZ_(label_name, target_ip)   GEN_JCC_MACRO(jz,  WriteJz,  label_name, target_ip)
#define JNS_(label_name, target_ip)   GEN_JCC_MACRO(jns,  WriteJns,  label_name, target_ip)

#define JMP_(label_name, target_ip) {                                                   \
    if (is_second_pass) fprintf(asm_file, "    jmp %s\n", label_name);                  \
    int32_t offset = (is_second_pass) ? (int32_t)(target_ip - (current_ip + 5)) : 0;    \
    current_ip += WriteJmp(offset, bin_file); \
}
#define JMP_R_(reg) {                                                                   \
    if (is_second_pass) fprintf(asm_file, "    jmp %s\n", about_registers_64[reg]);     \
    current_ip += WriteJmpR(reg, bin_file); \
}

#define LOOP_(label_name, target_ip) {                                                                  \
    if (is_second_pass) fprintf(asm_file, "    loop %s\n", label_name);                                 \
    int8_t offset = (is_second_pass) ? (int8_t)((int64_t)target_ip - (int64_t)(current_ip + 2)) : 0;    \
    current_ip += WriteLoop(offset, bin_file);                                                          \
}

#define SECTION_DATA_          fprintf(asm_file, "section .data\n"                                                      \
                                                 "    format_out      db \"Answer = %%d\", 10, 0\n"                     \
                                                 "    format_in       db \"%%lld\", 0\n\n"                              \
                                                 "    hex_chars       db \"0123456789ABCDEF\"\n"                        \
                                                 "    float_10        dd 10.0                     ; dword - 4 bytes\n"  \
                                                 "    float_mask_abs  dd 0x7FFFFFFF\n"                                  \
                                                 "    float_inf       db \"inf\", 0\n"                                  \
                                                 "    float_nan       db \"nan\", 0\n\n"                                \
                                                 "    jmp_table:\n"                                                     \
                                                 "        dq mprintf_case_b                               ; %%b\n"              \
                                                 "        dq mprintf_case_c                               ; %%c\n"              \
                                                 "        dq mprintf_case_d                               ; %%d\n"              \
                                                 "        times ('f' - 'd' - 1) dq mprintf_case_default   ; skip\n"             \
                                                 "        dq mprintf_case_f                               ; %%f\n"              \
                                                 "        times ('o' - 'f' - 1) dq mprintf_case_default   ; skip\n"             \
                                                 "        dq mprintf_case_o                               ; %%o\n"              \
                                                 "        times ('s' - 'o' - 1) dq mprintf_case_default   ; skip\n"             \
                                                 "        dq mprintf_case_s                               ; %%s\n"              \
                                                 "        times ('x' - 's' - 1) dq mprintf_case_default   ; skip\n"             \
                                                 "        dq mprintf_case_x                               ; %%x\n\n");

#define SECTION_BSS_            fprintf(asm_file, "section .bss\n"                                                      \
                                                  "    buf resb 1024                             ; reserve 1024 bytes\n"\
                                                  "    buf_len resq 1\n"                                                \
                                                  "    xmm_save resq 8\n"                                               \
                                                  "    buffer resb 32\n\n");
  

Tree_status BackEndX86(Language* language, const char* name_asm_file, const char* name_bin_file);

void CreateAsmFileX86(Language* language, Tree_node* tree_node, FILE* asm_file, FILE* bin_file);

void PrintOperationsX86(Language* language, Tree_node* tree_node, FILE* asm_file, FILE* bin_file, const char* operation);

void PrintOperationDivX86(Language* language, Tree_node* tree_node, FILE* asm_file, FILE* bin_file);

void PrintOperationSqrtX86(Language* language, Tree_node* tree_node, FILE* asm_file, FILE* bin_file);

void PrintAssignmentX86(Language* language, Tree_node* tree_node, FILE* asm_file, FILE* bin_file);

void PrintChangeX86(Language* language, Tree_node* tree_node, FILE* asm_file, FILE* bin_file);

void PrintIfX86(Language* language, Tree_node* tree_node, FILE* asm_file, FILE* bin_file);

void PrintElseX86(Language* language, Tree_node* tree_node, FILE* asm_file, FILE* bin_file, size_t if_id);

void PrintWhileX86(Language* language, Tree_node* tree_node, FILE* asm_file, FILE* bin_file);

void PrintInX86(Language* language, Tree_node* tree_node, FILE* asm_file, FILE* bin_file);

void PrintOutX86(Language* language, Tree_node* tree_node, FILE* asm_file, FILE* bin_file);

void PrintMainFunctionX86(Language* language, Tree_node* tree_node, FILE* asm_file, FILE* bin_file);

void PrintDefFunctionX86(Language* language, Tree_node* tree_node, FILE* asm_file, FILE* bin_file);

void PrintCallFunctionX86(Language* language, Tree_node* tree_node, FILE* asm_file, FILE* bin_file);

void PrintReturnFunctionX86(Language* language, Tree_node* tree_node, FILE* asm_file, FILE* bin_file);

void PrintFuncMyPrintf(Language* language, FILE* asm_file, FILE* bin_file);

void PrintFuncMyScanf(Language* language, FILE* asm_file, FILE* bin_file);


#endif // BACK_END_X86_H_