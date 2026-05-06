#ifndef BACK_END_X86_H_
#define BACK_END_X86_H_

#include "back_end_bin.h"
#include "../common.h"
#include "common_back.h"


#define IS_SECOND_PASS labels->is_second_pass
#define CURRENT_IP labels->current_ip

#define LST_(...) \
    if (IS_SECOND_PASS) fprintf(asm_file, __VA_ARGS__);

#define COMMAND_(func, ...) \
    CURRENT_IP += func(__VA_ARGS__, labels, bin_file);

#define COMMAND_NOARGS_(func) \
    CURRENT_IP += func(labels, bin_file);


#define PUSH_NUM_(num)            {LST_("    push %d\n", num)                                       COMMAND_(WritePushNum, num)}                         
#define PUSH_R_(reg)              {LST_("    push %s\n", regs_64[reg])                              COMMAND_(WritePushR, reg)}
#define PUSH_M_(reg)              {LST_("    push qword [%s]\n", regs_64[reg])                      COMMAND_(WritePushM, reg)}

#define POP_R_(reg)               {LST_("    pop %s\n", regs_64[reg])                               COMMAND_(WritePopR, reg)}
#define POP_M_(reg)               {LST_("    pop qword [%s]\n", regs_64[reg])                       COMMAND_(WritePopM, reg)}

#define MOV_R1_R2_(reg1, reg2)    {LST_("    mov %s, %s\n", regs_64[reg1], regs_64[reg2]);          COMMAND_(WriteMovR1R2, reg1, reg2)}
#define MOV_MR_R_(reg1, reg2)     {LST_("    mov [%s], %s\n", regs_64[reg1], regs_64[reg2]);        COMMAND_(WriteMovMrR, reg1, reg2)}
#define MOV_MR_R8_(reg1, reg2)    {LST_("    mov [%s], %s\n", regs_64[reg1], regs_8[reg2]);         COMMAND_(WriteMovMrR8, reg1, reg2)}
#define MOV_R_MR_(reg1, reg2)     {LST_("    mov %s, [%s]\n", regs_64[reg1], regs_64[reg2]);        COMMAND_(WriteMovRMr, reg1, reg2)}
#define MOV_R8_MR_(reg1, reg2)    {LST_("    mov %s, [%s]\n", regs_8[reg1], regs_64[reg2]);         COMMAND_(WriteMovR8Mr, reg1, reg2)}
#define MOV_R_ADDR_(reg, addr)    {LST_("    lea %s, [%s]\n", regs_64[reg], GetAddressName(addr));  COMMAND_(WriteMovRAddr, reg, addr)}
#define MOV_R_NUM_(reg, num)      {LST_("    mov %s, %ld\n", regs_64[reg], (int64_t)num);           COMMAND_(WriteMovRNum, reg, num)}
#define MOV_R8_CHAR_(reg, symb)   {LST_("    mov %s, '%c'\n", regs_8[reg], symb);                   COMMAND_(WriteMovR8Char, reg, symb)}
#define MOV_FORMAT_IN_(reg)       {LST_("    mov " #reg ", format_in\n");                           COMMAND_(WriteMovRNum, reg, ADDR_FORMAT_IN)}
#define MOV_FORMAT_OUT_(reg)      {LST_("    mov " #reg ", format_out\n");                          COMMAND_(WriteMovRNum, reg, ADDR_FORMAT_OUT)}
#define MOVQ_R_M_(xmm, reg)       {LST_("    movq %s, [%s]\n", xmms[xmm], regs_64[reg]);            COMMAND_(WriteMovqXmmMr, xmm, reg)}
#define MOVZX_R1_R2_(reg1, reg2)  {LST_("    movzx %s, %s\n", regs_64[reg1], regs_8[reg2]);         COMMAND_(WriteMovzxR1R2, reg1, reg2)}
#define MOVZX_R_M_(reg1, reg2)    {LST_("    movzx %s, byte [%s]\n", regs_64[reg1], regs_64[reg2]); COMMAND_(WriteMovzxRMr, reg1, reg2)}
#define MOVSXD_R1_R2_(reg1, reg2) {LST_("    movsxd %s, %s\n", regs_64[reg1], regs_32[reg2]);       COMMAND_(WriteMovsxdR1R2, reg1, reg2)}

#define SUB_R1_R2_(reg1, reg2)    {LST_("    sub %s, %s\n", regs_64[reg1], regs_64[reg2]);          COMMAND_(WriteSubR1R2, reg1, reg2)}
#define SUB_R_NUM_(reg, num)      {LST_("    sub %s, %d\n", regs_64[reg], num);                     COMMAND_(WriteSubRNum, reg, num)}                                         
#define DEC_(reg)                 {LST_("    dec %s\n", regs_64[reg]);                              COMMAND_(WriteDecR, reg)}

#define ADD_R1_R2_(reg1, reg2)    {LST_("    add %s, %s\n", regs_64[reg1], regs_64[reg2]);          COMMAND_(WriteAddR1R2, reg1, reg2)}
#define ADD_R_NUM_(reg, num)      {LST_("    add %s, %d\n", regs_64[reg], (num));                   COMMAND_(WriteAddRNum, reg, num)}
#define INC_(reg)                 {LST_("    inc %s\n", regs_64[reg]);                              COMMAND_(WriteIncR, reg)}

#define IDIV_R_(reg)              {LST_("    idiv %s\n", regs_64[reg]);                             COMMAND_(WriteIdivR, reg)}
#define DIV_R_(reg)               {LST_("    div %s\n", regs_64[reg]);                              COMMAND_(WriteDivR, reg)}
#define IMUL_R_NUM_(reg, num)     {LST_("    imul %s, %d\n", regs_64[reg], (num));                  COMMAND_(WriteImulRNum, reg, num)}
#define IMUL_R1_R2_(reg1, reg2)   {LST_("    imul %s, %s\n", regs_64[reg1], regs_64[reg2]);         COMMAND_(WriteImulR1R2, reg1, reg2)}

#define TEST_R1_R2_(reg1, reg2)   {LST_("    test %s, %s\n", regs_64[reg1], regs_64[reg2]);         COMMAND_(WriteTestR1R2, reg1, reg2)}
#define CMP_R1_R2_(reg1, reg2)    {LST_("    cmp %s, %s\n", regs_64[reg1], regs_64[reg2]);          COMMAND_(WriteCmpR1R2, reg1, reg2)}
#define CMP_R_NUM_(reg, num)      {LST_("    cmp %s, %d\n", regs_64[reg], num);                     COMMAND_(WriteCmpRNum, reg, num)}
#define CMP_R8_CHAR(reg, symb)    {if (IS_SECOND_PASS) {                                                            \
                                      if (symb > 31 && symb < 127) {LST_("    cmp %s, '%c'\n", regs_8[reg], symb)}  \
                                      else                         {LST_("    cmp %s, %d\n", regs_8[reg], symb);}   \
                                  }                                                                                 \
                                  COMMAND_(WriteCmpR8Char, reg, symb)}

#define AND_R1_R2_(reg1, reg2)    {LST_("    and %s, %s\n", regs_64[reg1], regs_64[reg2]);          COMMAND_(WriteAndR1R2, reg1, reg2)}
#define AND_R_NUM_(reg, num)      {LST_("    and %s, %d\n", regs_64[reg], num);                     COMMAND_(WriteAndRNum, reg, num)}
#define XOR_R1_R2_(reg1, reg2)    {LST_("    xor %s, %s\n", regs_64[reg1], regs_64[reg2]);          COMMAND_(WriteXorR1R2, reg1, reg2)}
#define SHL_(reg, num)            {LST_("    shl %s, %d\n", regs_64[reg], num);                     COMMAND_(WriteShlRNum, reg, num)}
#define NEG_(reg)                 {LST_("    neg %s\n", regs_64[reg]);                              COMMAND_(WriteNegR, reg)}
#define ROL_R_CL_(reg)            {LST_("    rol %s, cl\n", regs_64[reg]);                          COMMAND_(WriteRolRCl, reg)}

#define CVTSI2SD_(xmm, reg)       {LST_("    cvtsi2sd %s, %s\n", xmms[xmm], regs_64[reg]);          COMMAND_(WriteCvtsi2sd, xmm, reg)}
#define CVTTSD2SI_(reg, xmm)      {LST_("    cvttsd2si %s, %s\n", regs_64[reg], xmms[xmm]);         COMMAND_(WriteCvttsd2si, reg, xmm)}
#define SQRTSD_(xmm1, xmm2)       {LST_("    sqrtsd %s, %s\n", xmms[xmm1], xmms[xmm2]);             COMMAND_(WriteSqrtsd, xmm1, xmm2)}

#define CQO_                      {LST_("    cqo\n");                                               COMMAND_NOARGS_(WriteCqo)}
#define SYSCALL_                  {LST_("    syscall\n");                                           COMMAND_NOARGS_(WriteSyscall)}
#define RET_                      {LST_("    ret\n\n");                                             COMMAND_NOARGS_(WriteRet)}

#define LABEL_(name) {                                      \
    if (IS_SECOND_PASS) fprintf(asm_file, "%s:\n", name);   \
    else AddLabel(name, labels, CURRENT_IP);                \
}
#define DOT_LABEL_(name) {                                          \
    if (IS_SECOND_PASS) fprintf(asm_file, "%s:\n", DOT_L_(name));   \
    else AddLabel(DOT_L_(name), labels, CURRENT_IP);                \
}

#define FUNC_(name) {                                               \
    if (IS_SECOND_PASS) fprintf(asm_file, "\n%s:\n", name);         \
    else AddLabel(name, labels, CURRENT_IP);                        \
}    
#define CALL_(func) {                                                                                       \
    LST_( "    call %s\n", func);                                                                           \
    uint64_t target_addr = GetLabelAddress(func, labels);                                                   \
    int32_t offset = (IS_SECOND_PASS) ? (int32_t)((int64_t)target_addr - (int64_t)(CURRENT_IP + 5)) : 0;    \
    COMMAND_(WriteCall, offset)                                                                             \
}                      


#define GEN_JCC_MACRO(name, WriteJ, label_name, target_ip) {                                \
    LST_("    " #name " %s\n", label_name);                                                 \
    int32_t offset = (IS_SECOND_PASS) ? (int32_t)(target_ip - (CURRENT_IP + 6)) : 0;        \
    COMMAND_(WriteJ, offset)                                                                \
}
#define JLE_(label_name, target_ip)  GEN_JCC_MACRO(jle, WriteJle, label_name, target_ip)
#define JL_(label_name, target_ip)   GEN_JCC_MACRO(jl,  WriteJl,  label_name, target_ip)
#define JGE_(label_name, target_ip)  GEN_JCC_MACRO(jge, WriteJge, label_name, target_ip)
#define JG_(label_name, target_ip)   GEN_JCC_MACRO(jg,  WriteJg,  label_name, target_ip)
#define JNE_(label_name, target_ip)  GEN_JCC_MACRO(jne, WriteJne, label_name, target_ip)
#define JE_(label_name, target_ip)   GEN_JCC_MACRO(je,  WriteJe,  label_name, target_ip)
#define JNZ_(label_name, target_ip)  GEN_JCC_MACRO(jnz, WriteJnz, label_name, target_ip)
#define JZ_(label_name, target_ip)   GEN_JCC_MACRO(jz,  WriteJz,  label_name, target_ip)
#define JNS_(label_name, target_ip)  GEN_JCC_MACRO(jns, WriteJns, label_name, target_ip)

#define JMP_(label_name, target_ip) {                                                   \
    LST_("    jmp %s\n", label_name);                                                   \
    int32_t offset = (IS_SECOND_PASS) ? (int32_t)(target_ip - (CURRENT_IP + 5)) : 0;    \
    COMMAND_(WriteJmp, offset)                                                          \
}
#define JMP_R_(reg) {                       \
    LST_("    jmp %s\n", regs_64[reg]);     \
    COMMAND_(WriteJmpR, reg)                \
}

#define LOOP_(label_name, target_ip) {                                                                  \
    LST_("    loop %s\n", label_name);                                                                  \
    int8_t offset = (IS_SECOND_PASS) ? (int8_t)((int64_t)target_ip - (int64_t)(CURRENT_IP + 2)) : 0;    \
    COMMAND_(WriteLoop, offset)                                                                         \
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
  

Tree_status BackEndX86(Language* language, Labels* labels, const char* name_asm_file, const char* name_bin_file);

void CreateAsmFileX86(Language* language, Labels* labels, Tree_node* tree_node, FILE* asm_file, FILE* bin_file);

void PrintOperationsX86(Language* language, Labels* labels, Tree_node* tree_node, FILE* asm_file, FILE* bin_file, const char* operation);

void PrintOperationDivX86(Language* language, Labels* labels, Tree_node* tree_node, FILE* asm_file, FILE* bin_file);

void PrintOperationSqrtX86(Language* language, Labels* labels, Tree_node* tree_node, FILE* asm_file, FILE* bin_file);

void PrintOperationPow(Language* language, Labels* labels, Tree_node* tree_node, FILE* asm_file, FILE* bin_file);

void PrintAssignmentX86(Language* language, Labels* labels, Tree_node* tree_node, FILE* asm_file, FILE* bin_file);

void PrintChangeX86(Language* language, Labels* labels, Tree_node* tree_node, FILE* asm_file, FILE* bin_file);

void PrintIfX86(Language* language, Labels* labels, Tree_node* tree_node, FILE* asm_file, FILE* bin_file);

void PrintElseX86(Language* language, Labels* labels, Tree_node* tree_node, FILE* asm_file, FILE* bin_file, size_t if_id);

void PrintWhileX86(Language* language, Labels* labels, Tree_node* tree_node, FILE* asm_file, FILE* bin_file);

void PrintInX86(Language* language, Labels* labels, Tree_node* tree_node, FILE* asm_file, FILE* bin_file);

void PrintOutX86(Language* language, Labels* labels, Tree_node* tree_node, FILE* asm_file, FILE* bin_file);

void PrintMainFunctionX86(Language* language, Labels* labels, Tree_node* tree_node, FILE* asm_file, FILE* bin_file);

void PrintDefFunctionX86(Language* language, Labels* labels, Tree_node* tree_node, FILE* asm_file, FILE* bin_file);

void PrintCallFunctionX86(Language* language, Labels* labels, Tree_node* tree_node, FILE* asm_file, FILE* bin_file);

void PrintReturnFunctionX86(Language* language, Labels* labels, Tree_node* tree_node, FILE* asm_file, FILE* bin_file);

void PrintFuncMyPrintf(Language* language, Labels* labels, FILE* asm_file, FILE* bin_file);

void PrintFuncMyScanf(Language* language, Labels* labels, FILE* asm_file, FILE* bin_file);


#endif // BACK_END_X86_H_