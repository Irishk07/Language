#ifndef BACK_END_BIN_H_
#define BACK_END_BIN_H_

#include <stdint.h>


const int MAX_CNT_LABELS = 256;


enum DataOffsets {
    ADDR_BASE           = 0x400000,             // start base address
    ADDR_HEADERS_SIZE   = 0x100,                // 256 bytes for headers (Ehdr (64) + 3 * Phdr (56) = 232)
    
    // Section .data (Page 0)
    ADDR_FORMAT_OUT     = ADDR_BASE + ADDR_HEADERS_SIZE,
    ADDR_FORMAT_IN      = ADDR_FORMAT_OUT + 16,
    ADDR_HEX_CHARS      = ADDR_FORMAT_IN + 8,
    ADDR_FLOAT_10       = ADDR_HEX_CHARS + 20,
    ADDR_FLOAT_ABS      = ADDR_FLOAT_10 + 4,
    ADDR_FLOAT_INF      = ADDR_FLOAT_ABS + 4,
    ADDR_FLOAT_NAN      = ADDR_FLOAT_INF + 4,
    ADDR_JMP_TABLE      = ADDR_FLOAT_NAN + 8,
    // size of each segment 0x1000 = 4096 bytes = 4 Kbytes = 1 page
    // Section .bss (Page 1)
    ADDR_BSS_START      = 0x401000,
    ADDR_BUF            = ADDR_BSS_START,
    ADDR_BUF_LEN        = ADDR_BUF + 1024,
    ADDR_XMM_SAVE       = ADDR_BUF_LEN + 8,
    ADDR_BUFFER         = ADDR_XMM_SAVE + 64,
    
    // Section .text (Page 2)
    ADDR_ENTRY_POINT    = 0x402000 
};


enum registers_64 { 
    RAX = 0, RCX = 1, RDX = 2,  RBX = 3,  RSP = 4,  RBP = 5,  RSI = 6,  RDI = 7,
    R8  = 8, R9  = 9, R10 = 10, R11 = 11, R12 = 12, R13 = 13, R14 = 14, R15 = 15
};

[[maybe_unused]] static const char* about_registers_64[] {
    "rax", "rcx", "rdx", "rbx", "rsp", "rbp", "rsi", "rdi", 
    "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15"
};

enum registers_32 { 
    EAX = 0, ECX = 1, EDX = 2,  EBX = 3,  ESP = 4,  EBP = 5,  ESI = 6,  EDI = 7
};

[[maybe_unused]] static const char* about_registers_32[] {
    "eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi"
};

enum registers_8 {
    AL = 0, CL = 1, DL = 2, BL = 3,
};

[[maybe_unused]] static const char* about_registers_8[] {
    "al", "cl", "dl", "bl"
};

enum xmm { 
    XMM0 = 0, XMM1 = 1, XMM2 = 2, XMM3 = 3, XMM4 = 4, XMM5 = 5, XMM6 = 6, XMM7 = 7
};

[[maybe_unused]] static const char* about_xmm[] {
    "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6", "xmm7"
};

typedef struct {
    char name[128];
    uint64_t address;
} Label;


extern Label labels_table[MAX_CNT_LABELS];
extern size_t labels_count;
extern bool is_second_pass;
extern uint64_t current_ip;


void GenerateElf(FILE* bin_file); 
void FinalizeElfSize(FILE* bin_file);
void WriteJumpTable(FILE* bin_file);

uint64_t GetLabelAddress(const char* name);
void AddLabel(const char* name, uint64_t address);

const char* GetAddressName(uint32_t addr);

size_t WritePushR(registers_64 reg_id, FILE* bin_file);
size_t WritePushNum(int32_t num, FILE* bin_file);
size_t WritePushM(registers_64 reg_id, FILE* bin_file);

size_t WritePopR(registers_64 reg_id, FILE* bin_file);
size_t WritePopM(registers_64 reg_id, FILE* bin_file);

size_t WriteMovR1R2(registers_64 reg1, registers_64 reg2, FILE* bin_file);
size_t WriteMovMrR(registers_64 reg1, registers_64 reg2, FILE* bin_file);
size_t WriteMovMrR8(registers_64 reg_addr, registers_8 reg_src, FILE* bin_file);
size_t WriteMovRMr(registers_64 reg1, registers_64 reg2, FILE* bin_file);
size_t WriteMovR8Mr(registers_8 reg1, registers_64 reg2, FILE* bin_file);
size_t WriteMovRAddr(registers_64 dest_reg, uint64_t addr, FILE* bin_file);
size_t WriteMovRNum(registers_64 reg, int64_t num, FILE* bin_file);
size_t WriteMovR8Char(registers_8 reg, char symb, FILE* bin_file);
size_t WriteMovqXmmMr(xmm xmm_id, registers_64 reg_addr, FILE* bin_file);
size_t WriteMovzxR1R2(registers_64 reg1, registers_8 reg2, FILE* bin_file);
size_t WriteMovzxRMr(registers_64 reg1, registers_64 reg2, FILE* bin_file);
size_t WriteMovsxdR1R2(registers_64 reg1, registers_32 reg2, FILE* bin_file);

size_t WriteSubR1R2(registers_64 reg1, registers_64 reg2, FILE* bin_file);
size_t WriteSubRNum(registers_64 reg, int32_t num, FILE* bin_file);
size_t WriteDecR(registers_64 reg, FILE* bin_file);

size_t WriteAddR1R2(registers_64 reg1, registers_64 reg2, FILE* bin_file);
size_t WriteAddRNum(registers_64 reg, int32_t num, FILE* bin_file);
size_t WriteIncR(registers_64 reg, FILE* bin_file);

size_t WriteDivR(registers_64 reg, FILE* bin_file);
size_t WriteIdivR(registers_64 reg, FILE* bin_file);

size_t WriteImulRNum(registers_64 reg, int32_t num, FILE* bin_file);
size_t WriteImulR1R2(registers_64 reg1, registers_64 reg2, FILE* bin_file);

size_t WriteCmpR1R2(registers_64 r1, registers_64 r2, FILE* bin_file);
size_t WriteCmpRNum(registers_64 reg, int32_t num, FILE* bin_file);
size_t WriteCmpR8Char(registers_8 reg, uint8_t imm, FILE* bin_file);
size_t WriteTestR1R2(registers_64 r1, registers_64 r2, FILE* bin_file);

size_t WriteAndR1R2(registers_64 r1, registers_64 r2, FILE* bin_file);
size_t WriteAndRNum(registers_64 reg, int32_t num, FILE* bin_file);
size_t WriteXorR1R2(registers_64 r1, registers_64 r2, FILE* bin_file);
size_t WriteShlRNum(registers_64 reg, uint8_t num, FILE* bin_file);
size_t WriteNegR(registers_64 reg, FILE* bin_file);
size_t WriteRolRCl(registers_64 reg, FILE* bin_file);

size_t WriteCvtsi2sd(xmm xmm_idx, registers_64 reg_idx, FILE* bin_file);
size_t WriteCvttsd2si(registers_64 reg_idx, xmm xmm_idx, FILE* bin_file);

size_t WriteSqrtsd(xmm xmm1, xmm xmm2, FILE* bin_file);

size_t WriteCqo(FILE* bin_file);

size_t WriteJle(int32_t offset, FILE* bin_file);
size_t WriteJl(int32_t offset, FILE* bin_file);
size_t WriteJge(int32_t offset, FILE* bin_file);
size_t WriteJg(int32_t offset, FILE* bin_file);
size_t WriteJne(int32_t offset, FILE* bin_file);
size_t WriteJe(int32_t offset, FILE* bin_file);
size_t WriteJns(int32_t offset, FILE* bin_file);
size_t WriteJmp(int32_t offset, FILE* bin_file);
size_t WriteJmpR(registers_64 reg, FILE* bin_file);
size_t WriteJz(int32_t offset, FILE* bin_file);
size_t WriteJnz(int32_t offset, FILE* bin_file);
size_t WriteLoop(int8_t offset, FILE* bin_file);

size_t WriteRet(FILE* bin_file);
size_t WriteSyscall(FILE* bin_file);
size_t WriteCall(int32_t offset, FILE* bin_file);


#endif // BACK_END_BIN_H_