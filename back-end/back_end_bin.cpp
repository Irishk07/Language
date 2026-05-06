#include <assert.h>
#include <elf.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "back_end_bin.h"

Label labels_table[MAX_CNT_LABELS] = {}; 
size_t labels_count = 0;
bool is_second_pass = false;
uint64_t current_ip = 0;


void GenerateElf(FILE* bin_file) {
    assert(bin_file);

    const char* f_out = "Answer = %d\n";
    const char* f_in  = "%lld";
    const char* hex   = "0123456789ABCDEF";
    float f10 = 10.0f;
    uint32_t f_abs = 0x7FFFFFFF;

    // Header of ELF64 file
    Elf64_Ehdr ehdr = {
        .e_ident = {0x7f, 'E', 'L', 'F', 2, 1, 1, 0}, // magic bytes, 2 = 64-bit, 1 = little endian, 1 = version elf, 0 = OS (System Linux)
        .e_type = ET_EXEC,                            // type file - executable
        .e_machine = EM_X86_64,                       // architecture x86_84
        .e_version = EV_CURRENT,                      // current version of ELF (1)
        .e_entry = ADDR_ENTRY_POINT,                  // start code
        .e_phoff = sizeof(Elf64_Ehdr),                // offset programm headers (after Ehdr) (64 bytes)
        .e_ehsize = sizeof(Elf64_Ehdr),               // size of this header (64 bytes)
        .e_phentsize = sizeof(Elf64_Phdr),            // size pf one programm header (56 bytes)
        .e_phnum = 3                                  // 3 programm headers: data, bss, text
    };

    // Programm headers (data, bss, size)
    Elf64_Phdr phdr[3] = {};

    // Segment 0: Data (R)
    phdr[0].p_type   = PT_LOAD;     // load this segment to memory
    phdr[0].p_offset = 0;           // start read from 0 byte of file
    phdr[0].p_vaddr  = ADDR_BASE;   // load to address ADDR_BASE in memory
    phdr[0].p_filesz = 0x1000;      // give from file 4096 bytes 
    phdr[0].p_memsz  = 0x1000;      // occupy 4096 bytes in memory
    phdr[0].p_flags  = PF_R;        // only read
    phdr[0].p_align  = 0x1000;

    // Segment 1: BSS (RW)
    phdr[1].p_type   = PT_LOAD;
    phdr[1].p_offset = 0x1000;
    phdr[1].p_vaddr  = ADDR_BSS_START;
    phdr[1].p_filesz = 0x1000;
    phdr[1].p_memsz  = 0x1000;
    phdr[1].p_flags  = PF_R | PF_W;
    phdr[1].p_align  = 0x1000;

    // Segment 2: Text (RX)
    phdr[2].p_type   = PT_LOAD;
    phdr[2].p_offset = 0x2000;
    phdr[2].p_vaddr  = ADDR_ENTRY_POINT;
    phdr[2].p_filesz = 0;           // don't know now, fix later
    phdr[2].p_memsz  = 0;           // don't know now, fix later
    phdr[2].p_flags  = PF_R | PF_X;
    phdr[2].p_align  = 0x1000;

    rewind(bin_file);                               // move pointer in file to the begin of file
    fwrite(&ehdr, sizeof(ehdr), 1, bin_file);       // write ELF64 header
    fwrite(phdr, sizeof(Elf64_Phdr), 3, bin_file);  // write programm headers

    fseek(bin_file, 0x2000 - 1, SEEK_SET);          // move pointer in file to the end of second page (first segment bss)
    fputc(0, bin_file);                             // write one null byte

    fseek(bin_file, ADDR_FORMAT_OUT - ADDR_BASE, SEEK_SET);
    fwrite(f_out, 1, strlen(f_out) + 1, bin_file);

    fseek(bin_file, ADDR_FORMAT_IN - ADDR_BASE, SEEK_SET);
    fwrite(f_in, 1, strlen(f_in) + 1, bin_file);

    fseek(bin_file, ADDR_HEX_CHARS - ADDR_BASE, SEEK_SET);
    fwrite(hex, 1, strlen(hex) + 1, bin_file);

    fseek(bin_file, ADDR_FLOAT_10 - ADDR_BASE, SEEK_SET);
    fwrite(&f10, sizeof(float), 1, bin_file);

    fseek(bin_file, ADDR_FLOAT_ABS - ADDR_BASE, SEEK_SET);
    fwrite(&f_abs, sizeof(uint32_t), 1, bin_file);

    fseek(bin_file, 0x2000, SEEK_SET);
}


void FinalizeElfSize(FILE* bin_file) {
    assert(bin_file);

    fseek(bin_file, 0, SEEK_END);       // move pointer in file to the end of file
    long final_pos = ftell(bin_file);   // current position in file => we have size of file

    size_t code_real_size = (size_t)final_pos - (ADDR_ENTRY_POINT - ADDR_BASE);

    long phdr_2_offset = sizeof(Elf64_Ehdr) + 2 * sizeof(Elf64_Phdr);
    long phdr_filesz_pos = (long)phdr_2_offset + (long)offsetof(Elf64_Phdr, p_filesz);  // offsetof: give byte of p_filesz in struct Phdr
    fseek(bin_file, phdr_filesz_pos, SEEK_SET);
    
    fwrite(&code_real_size, 8, 1, bin_file); // p_filesz
    fwrite(&code_real_size, 8, 1, bin_file); // p_memsz
}

void WriteJumpTable(FILE* bin_file) {
    assert(bin_file);

    fseek(bin_file, ADDR_JMP_TABLE - ADDR_BASE, SEEK_SET);

    uint64_t addr_b       = GetLabelAddress("mprintf_case_b");
    uint64_t addr_c       = GetLabelAddress("mprintf_case_c");
    uint64_t addr_d       = GetLabelAddress("mprintf_case_d");
    uint64_t addr_f       = GetLabelAddress("mprintf_case_f");
    uint64_t addr_o       = GetLabelAddress("mprintf_case_o");
    uint64_t addr_s       = GetLabelAddress("mprintf_case_s");
    uint64_t addr_x       = GetLabelAddress("mprintf_case_x");
    uint64_t addr_default = GetLabelAddress("mprintf_case_default");

    for (char c = 'b'; c <= 'x'; ++c) {
        uint64_t current_addr = 0;

        switch (c) {
            case 'b': current_addr = addr_b; break;
            case 'c': current_addr = addr_c; break;
            case 'd': current_addr = addr_d; break;
            case 'f': current_addr = addr_f; break;
            case 'o': current_addr = addr_o; break;
            case 's': current_addr = addr_s; break;
            case 'x': current_addr = addr_x; break;
            default:  current_addr = addr_default; break;
        }

        fwrite(&current_addr, sizeof(uint64_t), 1, bin_file);
    }
}

static void WriteByte(uint8_t byte, FILE* bin_file) { 
    if (bin_file && is_second_pass)
        fwrite(&byte, 1, 1, bin_file);
}

static void WriteInt32(int32_t val, FILE* bin_file) {
    if (!bin_file || !is_second_pass)
        return;

    // Little-endian:
    WriteByte((uint8_t)(val & 0xFF),         bin_file); // last 8 bits
    WriteByte((uint8_t)((val >> 8) & 0xFF),  bin_file);
    WriteByte((uint8_t)((val >> 16) & 0xFF), bin_file);
    WriteByte((uint8_t)((val >> 24) & 0xFF), bin_file);
}

uint64_t GetLabelAddress(const char* name) {
    assert(name);

    for (size_t i = 0; i < labels_count; ++i) {
        if (strcmp(labels_table[i].name, name) == 0)
            return labels_table[i].address;
    }

    return 0;
}

void AddLabel(const char* name, uint64_t address) {
    assert(name);

    if (labels_count < MAX_CNT_LABELS) {
        strcpy(labels_table[labels_count].name, name);
        labels_table[labels_count].address = address;
        labels_count++;
    }
}

const char* GetAddressName(uint32_t addr) {
    switch(addr) {
        case ADDR_JMP_TABLE:  return "jmp_table";
        case ADDR_HEX_CHARS:  return "hex_chars";
        case ADDR_FORMAT_OUT: return "format_out";
        case ADDR_FORMAT_IN:  return "format_in";
        case ADDR_BUF:        return "buf";
        case ADDR_BUF_LEN:    return "buf_len";
        case ADDR_XMM_SAVE:   return "xmm_save";
        case ADDR_BUFFER:     return "buffer";
        default:              return "unknown_addr";
    }
}

// push r
size_t WritePushR(registers_64 reg_id, FILE* bin_file) {
    if (reg_id >= R8)
        WriteByte(0x41, bin_file);  // REX prefix
    WriteByte(0x50 + (reg_id & 7), bin_file); // 0x50 = opcode push; reg_id & 7: reg_id -> [0; 7];
    
    return (reg_id >= R8) ? 2 : 1;
}

// push num
size_t WritePushNum(int32_t num, FILE* bin_file) {
    if (num >= -128 && num <= 127) {        // if one signed byte
        WriteByte(0x6A, bin_file);          // 0x6A = opcode push imm8
        WriteByte((uint8_t)num, bin_file);

        return 2;
    } else {
        WriteByte(0x68, bin_file);          // 0x68 = opcode push imm32
        WriteInt32(num, bin_file);

        return 5;
    }
}

// push [r]
size_t WritePushM(registers_64 reg_id, FILE* bin_file) {
    size_t size = 0;
    if (reg_id >= R8) {
        WriteByte(0x41, bin_file); // REX prefix
        size++;
    }

    WriteByte(0xFF, bin_file);      // opcode of group instrctions which use memory (push, inc, dec), Reg = 6 if push
    size++;

    // byte ModR/M: [Mod: 2 bits] [Reg: 3 bits] [R/M: 3 bits]
    // Mod: 0x00 << 6  - 0x00 becaue we use [reg] without offset (7-6 bits)
    // Reg: 6 << 3     - 6 for 0xFF because push (5-3 bits)
    // R/M: reg_id & 7 - reg_id -> [0, 7] (2-0 bits)
    uint8_t modrm = (uint8_t)((0x00 << 6) | (6 << 3) | (reg_id & 7));
    WriteByte(modrm, bin_file);
    size++;

    // if reg_id & 7 == 4 => proc wait SIB (Scale-Index-Base) byte (r12/rsp)
    // for example mov rax, [rbx + rcx * 4 + 0x10]
    if ((reg_id & 7) == 4) {
        WriteByte(0x24, bin_file); // 0x24 = SIB: base = [reg], no index
        size++;
    } 

    // if reg_id & 7 == 5 && mod == 00 => proc think that we use absolute address (rip-relative) (r13/rbp)
    // for example mox rax, [rel buf]
    else if ((reg_id & 7) == 5) {
        // Change Mov: 01 - offset 1 byte
        if (bin_file && is_second_pass)
            fseek(bin_file, -1, SEEK_CUR); 
        modrm = (uint8_t)((0x01 << 6) | (6 << 3) | (reg_id & 7));
        WriteByte(modrm, bin_file);
        
        // 00 = offset (we have [r13 + 0]/[rbp + 0])
        WriteByte(0x00, bin_file);
        size++;
    }

    return size;
}

// pop r
// work such as push r
size_t WritePopR(registers_64 reg_id, FILE* bin_file) {
    if (reg_id >= R8)
        WriteByte(0x41, bin_file);

    WriteByte(0x58 + (reg_id & 7), bin_file);   // 0x58 = opcode pop
    return (reg_id >= R8) ? 2 : 1;
}

// pop [r]
size_t WritePopM(registers_64 reg_id, FILE* bin_file) {
    size_t size = 0;
    if (reg_id >= R8) {
        WriteByte(0x41, bin_file); // REX prefix
        size++;
    }

    WriteByte(0x8F, bin_file);     // 0x8F = opcode of group instructions 
    size++;

    // byte ModR/M: [Mod: 2 bits] [Reg: 3 bits] [R/M: 3 bits]
    // Mod: 0x00 << 6  - 0x00 because we use [reg] without offset (7-6 bits)
    // Reg: 0 << 3     - 0 for 0x8F because pop (5-3 bits)
    // R/M: reg_id & 7 - reg_id -> [0, 7] (2-0 bits)
    uint8_t modrm = (uint8_t)((0x00 << 6) | (0 << 3) | (reg_id & 7));
    WriteByte(modrm, bin_file);
    size++;

    // such as push [r]
    if ((reg_id & 7) == 4) {
        WriteByte(0x24, bin_file);
        size++;
    } 
    else if ((reg_id & 7) == 5) {
        if (bin_file && is_second_pass)
            fseek(bin_file, -1, SEEK_CUR);
        modrm = (uint8_t)((0x01 << 6) | (0 << 3) | (reg_id & 7));
        WriteByte(modrm, bin_file);
        
        WriteByte(0x00, bin_file);
        size++;
    }

    return size;
}

// mov r1, r2
size_t WriteMovR1R2(registers_64 reg1, registers_64 reg2, FILE* bin_file) {
    uint8_t rex = 0x48;          // use 64-bit regs
    if (reg2 >= R8) rex |= 0x04; // REX.R (+8 reg2)
    if (reg1 >= R8) rex |= 0x01; // REX.B (+8 reg1)

    WriteByte(rex, bin_file);
    WriteByte(0x89, bin_file);   // 0x89 = opcode mov r1, r2

    // Mod: 0xC0 (11)  - 0xC0 because we mov r1, r2, without memory (7-6 bits)
    // Reg: reg2 & 7   - reg2 -> [0, 7] (5-3 bits)
    // R/M: reg1 & 7   - reg1 -> [0, 7] (2-0 bits)
    uint8_t modrm = (uint8_t)(0xC0 | ((reg2 & 7) << 3) | (reg1 & 7));
    WriteByte(modrm, bin_file);

    return 3;
}

// mov [r1], r2
size_t WriteMovMrR(registers_64 reg1, registers_64 reg2, FILE* bin_file) {
    uint8_t rex = 0x48;          // use 64-bit regs
    if (reg2 >= R8) rex |= 0x04; // REX.R
    if (reg1 >= R8) rex |= 0x01; // REX.B

    WriteByte(rex, bin_file);
    WriteByte(0x89, bin_file);   // 0x89 = opcode mov [r1], r2

    // if dst R13/RBP (rip-relative)
    if ((reg1 & 7) == 5) {
        // Mod: 0x00       - 0x01 because we mov [r1 + 0], r2, with offset 1 byte (7-6 bits)
        // Reg: reg2 & 7   - reg2 -> [0, 7] (5-3 bits)
        // R/M: reg1 & 7   - reg1 -> [0, 7] (2-0 bits)
        uint8_t modrm = (uint8_t)(0x01 << 6 | ((reg2 & 7) << 3) | (reg1 & 7));
        WriteByte(modrm, bin_file);
        WriteByte(0x00, bin_file); // offset 0

        return 4;
    }

    // Mod: 0x00 (00)  - 0x00 because we mov [r1], r2, without offset (7-6 bits)
    // Reg: reg2 & 7   - reg2 -> [0, 7] (5-3 bits)
    // R/M: reg1 & 7   - reg1 -> [0, 7] (2-0 bits)
    uint8_t modrm = (uint8_t)(0x00 | ((reg2 & 7) << 3) | (reg1 & 7));
    WriteByte(modrm, bin_file);
    
    // if dst R12/RSP (SIB)
    if ((reg1 & 7) == 4) {
        WriteByte(0x24, bin_file);
        return 4;
    }

    return 3;
}

// mov [r1], r2
// r2 8-bit
size_t WriteMovMrR8(registers_64 reg_addr, registers_8 reg_src, FILE* bin_file) {
    size_t size = 0;
    uint8_t rex = 0x40;         // because src 8-bit
    if (reg_addr >= R8) {
        rex |= 0x01;
        WriteByte(rex, bin_file);
        size++;
    }

    WriteByte(0x88, bin_file);  // 0x88 = opcode mov [r1], r2 where r2 8-bit
    size++;

    // if dst R13/RBP (rip-relative)
    if ((reg_addr & 7) == 5) {
        // Mod: 0x01       - 0x01 because we mov [r1 + 0], r2, with offset 1 byte (7-6 bits)
        // Reg: reg2 & 7   - reg2 -> [0, 7] (5-3 bits)
        // R/M: reg1 & 7   - reg1 -> [0, 7] (2-0 bits)
        uint8_t modrm = (uint8_t)((0x01 << 6) | ((reg_src & 7) << 3) | (reg_addr & 7));
        WriteByte(modrm, bin_file);
        WriteByte(0x00, bin_file);

        return size + 2;
    }

    // Mod: 0x00 (00)  - 0x00 because we mov [r1], r2, without offset (7-6 bits)
    // Reg: reg2 & 7   - reg2 -> [0, 7] (5-3 bits)
    // R/M: reg1 & 7   - reg1 -> [0, 7] (2-0 bits)
    uint8_t modrm = (uint8_t)((0x00 << 6) | ((reg_src & 7) << 3) | (reg_addr & 7));
    WriteByte(modrm, bin_file);
    size++;

    // if dst R12/RSP (SIB)
    if ((reg_addr & 7) == 4) {
        WriteByte(0x24, bin_file);
        size++;
    }

    return size;
}

// mov r1, [r2]
size_t WriteMovRMr(registers_64 reg1, registers_64 reg2, FILE* bin_file) {
    uint8_t rex = 0x48;             // use 64-bit regs
    if (reg1 >= R8) rex |= 0x04; 
    if (reg2 >= R8) rex |= 0x01;
    WriteByte(rex, bin_file);

    WriteByte(0x8B, bin_file);     // 0x8B = opcode mov r1, [r2]

    // if src R13/RBP (rip-relative)
    if ((reg2 & 7) == 5) {
        // Mod: 0x01       - 0x01 because we mov [r1 + 0], r2, with offset 1 byte (7-6 bits)
        // Reg: reg2 & 7   - reg2 -> [0, 7] (5-3 bits)
        // R/M: reg1 & 7   - reg1 -> [0, 7] (2-0 bits)
        uint8_t modrm = (uint8_t)((0x01 << 6) | ((reg1 & 7) << 3) | (reg2 & 7));
        WriteByte(modrm, bin_file);
        WriteByte(0x00, bin_file); // offset 0
        
        return 5; 
    }

    // Mod: 0x00 (00)  - 0x00 because we mov r1, [r2], without offset (7-6 bits)
    // Reg: reg2 & 7   - reg2 -> [0, 7] (5-3 bits)
    // R/M: reg1 & 7   - reg1 -> [0, 7] (2-0 bits)
    uint8_t modrm = (uint8_t)(0x00 | ((reg1 & 7) << 3) | (reg2 & 7));
    WriteByte(modrm, bin_file);

    // if src R12/RSP (SIB)
    if ((reg2 & 7) == 4) {
        WriteByte(0x24, bin_file);

        return 4;
    }

    return 3;
}

// mov r1, [r2]
// r1 8-bit
size_t WriteMovR8Mr(registers_8 reg1, registers_64 reg2, FILE* bin_file) {
    size_t size = 0;
    uint8_t rex = 0x40;         // because dst 8-bit
    if (reg2 >= R8) {
        rex |= 0x01;
        WriteByte(rex, bin_file);
        size++;
    }

    WriteByte(0x8A, bin_file);  // 0x8A = opcode mov r1, [r2] where r1 8-bit
    size++;

    // if src R13/RBP (rip-relative)
    if ((reg2 & 7) == 5) {
        uint8_t modrm = (uint8_t)((0x01 << 6) | ((reg1 & 7) << 3) | (reg2 & 7));
        WriteByte(modrm, bin_file);
        WriteByte(0x00, bin_file);
        return size + 2;
    }

    uint8_t modrm = (uint8_t)((0x00 << 6) | ((reg1 & 7) << 3) | (reg2 & 7));
    WriteByte(modrm, bin_file);
    size++;

    // if src R12/RSP (SIB)
    if ((reg2 & 7) == 4) {
        WriteByte(0x24, bin_file);
        size++;
    }

    return size;
}

// lea r, [addr]
size_t WriteMovRAddr(registers_64 dest_reg, uint64_t addr, FILE* bin_file) {
    uint8_t rex = 0x48;
    if (dest_reg >= R8) rex |= 0x01; 
    
    WriteByte(rex, bin_file);
    WriteByte(0xB8 + (dest_reg & 7), bin_file);     // 0xB8 opcode mov r, addr

    if (bin_file && is_second_pass)
        fwrite(&addr, 8, 1, bin_file);

    return 10;
}

// mor r, num
size_t WriteMovRNum(registers_64 reg, int64_t num, FILE* bin_file) {
    uint8_t rex = 0x48;
    if (reg >= R8) rex |= 0x01;

    WriteByte(rex, bin_file);
    WriteByte(0xB8 + (reg & 7), bin_file);          // 0xB8 opcode mov r, num

    if (bin_file && is_second_pass)
        fwrite(&num, 8, 1, bin_file); 

    return 10;
}

// mov r, char
// r 8-bit
size_t WriteMovR8Char(registers_8 reg, char symb, FILE* bin_file) {
    WriteByte((uint8_t)(0xB0 + (reg & 7)), bin_file); // 0xB0 opcode mov r, char
    WriteByte((uint8_t)symb, bin_file);

    return 2;
}

// movq xmm, [r]
size_t WriteMovqXmmMr(xmm xmm_id, registers_64 reg_addr, FILE* bin_file) {
    WriteByte(0x66, bin_file);      // 0x66 - we work with xmm

    uint8_t rex = 0x48; 
    if (reg_addr >= R8) rex |= 0x01;
    WriteByte(rex, bin_file);

    // 0x0F 0x7E - opcode of movq
    WriteByte(0x0F, bin_file);
    WriteByte(0x7E, bin_file);

    uint8_t modrm = (uint8_t)(0x00 | ((xmm_id & 7) << 3) | (reg_addr & 7));
    WriteByte(modrm, bin_file);

    return 5;
}

// movzx r1, r2
// r2 8-bit
size_t WriteMovzxR1R2(registers_64 reg1, registers_8 reg2, FILE* bin_file) {
    size_t size = 0;
    uint8_t rex = 0x48;         // because result 8-bit
    if (reg1 >= R8) {
        rex |= 0x04;
        WriteByte(rex, bin_file);
        size++;
    }

    // 0x0F 0xB6 = opcode movzx r1, r2 where r2 8-bit
    WriteByte(0x0F, bin_file);
    WriteByte(0xB6, bin_file);

    uint8_t modrm = (uint8_t)(0xC0 | ((reg1 & 7) << 3) | (reg2 & 7));
    WriteByte(modrm, bin_file);

    return size + 3;
}

// movzx r1, byte [r2]
size_t WriteMovzxRMr(registers_64 reg1, registers_64 reg2, FILE* bin_file) {
    uint8_t rex = 0x48;
    if (reg1 >= R8) rex |= 0x04;
    if (reg2 >= R8) rex |= 0x01;
    WriteByte(rex, bin_file);

    // 0x0F 0xB6 = opcode movzx r1, byte [r2]
    WriteByte(0x0F, bin_file);
    WriteByte(0xB6, bin_file);

    uint8_t modrm = (uint8_t)(0x00 | ((reg1 & 7) << 3) | (reg2 & 7));
    WriteByte(modrm, bin_file);

    return 4;
}

// movsxd r1, r2
// r2 32-bit
size_t WriteMovsxdR1R2(registers_64 reg1, registers_32 reg2, FILE* bin_file) {
    uint8_t rex = 0x48;
    if (reg1 >= R8) rex |= 0x04;
    WriteByte(rex, bin_file);

    WriteByte(0x63, bin_file);  // 0x63 = opcode movsxd r1, r2 where r2 32-bit

    uint8_t modrm = (uint8_t)(0xC0 | ((reg1 & 7) << 3) | (reg2 & 7));
    WriteByte(modrm, bin_file);

    return 3;
}

// sub r1, r2
size_t WriteSubR1R2(registers_64 reg1, registers_64 reg2, FILE* bin_file) {
    uint8_t rex = 0x48;
    if (reg2 >= R8) rex |= 0x04; 
    if (reg1 >= R8) rex |= 0x01; 
    WriteByte(rex, bin_file);

    WriteByte(0x29, bin_file); // 0x29 = opcode sub r1, r2

    uint8_t modrm = (uint8_t)(0xC0 | ((reg2 & 7) << 3) | (reg1 & 7));
    WriteByte(modrm, bin_file);

    return 3;
}

// sub r, num
size_t WriteSubRNum(registers_64 reg, int32_t num, FILE* bin_file) {
    uint8_t rex = 0x48;
    if (reg >= R8) rex |= 0x01; 
    WriteByte(rex, bin_file);

    if (num >= -128 && num <= 127) {    // 1 byte
        WriteByte(0x83, bin_file);      // 0x83 = opcode of short form
        // Mod: 11         - 11 because we sub r1, num, without memory (7-6 bits)
        // Reg: 101        - opcode extension 101 (5 = sub)
        // R/M: reg &  7   - reg -> [0, 7] (2-0 bits)
        WriteByte((uint8_t)(0xC0 | (5 << 3) | (reg & 7)), bin_file); 
        WriteByte((uint8_t)num, bin_file);

        return 4;
    } else {
        WriteByte(0x81, bin_file);      // 0x81 = opcode of long form
        WriteByte((uint8_t)(0xC0 | (5 << 3) | (reg & 7)), bin_file);
        WriteInt32(num, bin_file);

        return 7;
    }
}

// dec r
size_t WriteDecR(registers_64 reg, FILE* bin_file) {
    uint8_t rex = 0x48;
    if (reg >= R8) rex |= 0x01;
    WriteByte(rex, bin_file);

    WriteByte(0xFF, bin_file); // opcode of group instructions

    // Mod: 11         - 11 because we dec r without memory (7-6 bits)
    // Reg: 001        - opcode extension 001 (1 = dec)
    // R/M: reg & 7   - reg -> [0, 7] (2-0 bits)
    uint8_t modrm = (uint8_t)(0xC0 | (1 << 3) | (reg & 7)); 
    WriteByte(modrm, bin_file);
    return 3;
}

// add r1, r2
size_t WriteAddR1R2(registers_64 reg1, registers_64 reg2, FILE* bin_file) {
    uint8_t rex = 0x48;
    if (reg2 >= R8) rex |= 0x04; 
    if (reg1 >= R8) rex |= 0x01; 
    WriteByte(rex, bin_file);

    WriteByte(0x01, bin_file); // 0x01 opcode add r1, r2

    uint8_t modrm = (uint8_t)(0xC0 | ((reg2 & 7) << 3) | (reg1 & 7));
    WriteByte(modrm, bin_file);

    return 3;
}

size_t WriteAddRNum(registers_64 reg, int32_t num, FILE* bin_file) {
    uint8_t rex = 0x48;
    if (reg >= R8) rex |= 0x01; 
    WriteByte(rex, bin_file);

    if (num >= -128 && num <= 127) {    // 1 byte
        WriteByte(0x83, bin_file);      // 0x83 = opcode of short form
        // Mod: 11         - 11 because we add r, num, without memory (7-6 bits)
        // Reg: 000        - opcode extension 000 (0 = add)
        // R/M: reg &  7   - reg -> [0, 7] (2-0 bits)
        WriteByte((uint8_t)(0xC0 | (reg & 7)), bin_file);
        WriteByte((uint8_t)num, bin_file);

        return 4;
    } else {
        WriteByte(0x81, bin_file);      // 0x81 = opcode of long form
        WriteByte((uint8_t)(0xC0 | (reg & 7)), bin_file);
        WriteInt32(num, bin_file);

        return 7;
    }
}

// inc r
size_t WriteIncR(registers_64 reg, FILE* bin_file) {
    uint8_t rex = 0x48;
    if (reg >= R8) rex |= 0x01;
    WriteByte(rex, bin_file);

    WriteByte(0xFF, bin_file); // opcode of group instructions

    // Mod: 11         - 11 because we inc r without memory (7-6 bits)
    // Reg: 000        - opcode extension 000 (0 = inc)
    // R/M: reg & 7    - reg -> [0, 7] (2-0 bits)
    uint8_t modrm = (uint8_t)(0xC0 | (reg & 7)); 
    WriteByte(modrm, bin_file);
    return 3;
}

// div r
size_t WriteDivR(registers_64 reg, FILE* bin_file) {
    uint8_t rex = 0x48; 
    if (reg >= R8) rex |= 0x01;
    WriteByte(rex, bin_file);

    WriteByte(0xF7, bin_file);  // 0xF7 = opcode div/idiv

    // Mod: 11         - 11 because we div r without memory (7-6 bits)
    // Reg: 110        - opcode extension 110 (6 = div)
    // R/M: reg & 7    - reg -> [0, 7] (2-0 bits)
    uint8_t modrm = (uint8_t)(0xC0 | (6 << 3) | (reg & 7));
    WriteByte(modrm, bin_file);

    return 3;
}

// idiv r
size_t WriteIdivR(registers_64 reg, FILE* bin_file) {
    uint8_t rex = 0x48;
    if (reg >= R8) rex |= 0x01;
    WriteByte(rex, bin_file);

    WriteByte(0xF7, bin_file);   // 0xF7 = opcode div/idiv

    // Mod: 11         - 11 because we div r without memory (7-6 bits)
    // Reg: 111        - opcode extension 111 (7 = idiv)
    // R/M: reg & 7    - reg -> [0, 7] (2-0 bits)
    uint8_t modrm = (uint8_t)(0xC0 | (7 << 3) | (reg & 7));
    WriteByte(modrm, bin_file);

    return 3;
}

// imul r, r, num
size_t WriteImulRNum(registers_64 reg, int32_t num, FILE* bin_file) {
    uint8_t rex = 0x48;
    if (reg >= R8) {
        rex |= 0x04; 
        rex |= 0x01;
    }
    WriteByte(rex, bin_file);

    if (num >= -128 && num <= 127) {    // 1 byte
        WriteByte(0x6B, bin_file);      // 0x6B = opcode short imul
        uint8_t modrm = (uint8_t)(0xC0 | ((reg & 7) << 3) | (reg & 7));
        WriteByte(modrm, bin_file);
        WriteByte((uint8_t)num, bin_file);

        return 4;
    } else {
        WriteByte(0x69, bin_file);      // 0x69 = opcode long imul
        uint8_t modrm = (uint8_t)(0xC0 | ((reg & 7) << 3) | (reg & 7));
        WriteByte(modrm, bin_file);
        WriteInt32(num, bin_file);

        return 7;
    }
}

// imul r1, r2
size_t WriteImulR1R2(registers_64 reg1, registers_64 reg2, FILE* bin_file) {
    uint8_t rex = 0x48; 
    if (reg1 >= R8) rex |= 0x04; 
    if (reg2 >= R8) rex |= 0x01; 
    WriteByte(rex, bin_file);

    WriteByte(0x0F, bin_file);
    WriteByte(0xAF, bin_file);
    
    uint8_t modrm = (uint8_t)(0xC0 | ((reg1 & 7) << 3) | (reg2 & 7));
    WriteByte(modrm, bin_file);

    return 4; 
}

// cmp r1, r2
size_t WriteCmpR1R2(registers_64 r1, registers_64 r2, FILE* bin_file) {
    uint8_t rex = 0x48; 
    if (r2 >= R8) rex |= 0x04; 
    if (r1 >= R8) rex |= 0x01; 
    WriteByte(rex, bin_file);

    WriteByte(0x39, bin_file);  // 0x39 = opcode of cmp r1, r2
    WriteByte((uint8_t)(0xC0 | ((r2 & 7) << 3) | (r1 & 7)), bin_file); 

    return 3;
}

// cmp r, num
size_t WriteCmpRNum(registers_64 reg, int32_t num, FILE* bin_file) {
    uint8_t rex = 0x48;
    if (reg >= R8) rex |= 0x01;
    WriteByte(rex, bin_file);

    WriteByte(0x81, bin_file);  // 0x81 = opcode of group instructions

    // Mod: 11         - 11 because we cmp r, num without memory (7-6 bits)
    // Reg: 111        - opcode extension 111 (7 = cmp)
    // R/M: reg & 7    - reg -> [0, 7] (2-0 bits)
    WriteByte((uint8_t)(0xC0 | (7 << 3) | (reg & 7)), bin_file);
    WriteInt32(num, bin_file);

    return 7;
}

// cmp r, char
// r 8-bit
size_t WriteCmpR8Char(registers_8 reg, uint8_t num, FILE* bin_file) {
    WriteByte(0x80, bin_file);  // 0x80 = opcode of group instructions (with 8-bit)

    // Mod: 11         - 11 because we cmp r, num without memory (7-6 bits)
    // Reg: 111        - opcode extension 111 (7 = cmp)
    // R/M: reg & 7    - reg -> [0, 7] (2-0 bits)
    WriteByte((uint8_t)(0xC0 | (7 << 3) | (reg & 7)), bin_file);
    WriteByte(num, bin_file);

    return 3;
}

// test r1, r2
size_t WriteTestR1R2(registers_64 reg1, registers_64 reg2, FILE* bin_file) {
    uint8_t rex = 0x48;
    if (reg1 >= R8) rex |= 0x04;
    if (reg2 >= R8) rex |= 0x01;
    WriteByte(rex, bin_file);

    WriteByte(0x85, bin_file);  // 0x85 = opcode test r1, r1
    WriteByte((uint8_t)(0xC0 | ((reg1 & 7) << 3) | (reg2 & 7)), bin_file);

    return 3;
}

// and r1, r2
size_t WriteAndR1R2(registers_64 reg1, registers_64 reg2, FILE* bin_file) {
    uint8_t rex = 0x48;
    if (reg1 >= R8) rex |= 0x04;
    if (reg2 >= R8) rex |= 0x01;
    WriteByte(rex, bin_file);

    WriteByte(0x21, bin_file);  // 0x21 = opcode test r1, r1
    WriteByte((uint8_t)(0xC0 | ((reg1 & 7) << 3) | (reg2 & 7)), bin_file);

    return 3;
}

// and r, num
size_t WriteAndRNum(registers_64 reg, int32_t num, FILE* bin_file) {
    uint8_t rex = 0x48;
    if (reg >= R8) rex |= 0x01;
    WriteByte(rex, bin_file);

    WriteByte(0x81, bin_file); // 0x81 = opcode of group instructions
    // Mod: 11         - 11 because we and r, num, without memory (7-6 bits)
    // Reg: 100        - opcode extension 100 (4 = and)
    // R/M: reg &  7   - reg -> [0, 7] (2-0 bits)
    WriteByte(0xC0 | (4 << 3) | (reg & 7), bin_file);
    
    WriteInt32(num, bin_file);

    return 7;
}

// xor r1, r2
size_t WriteXorR1R2(registers_64 reg1, registers_64 reg2, FILE* bin_file) {
    uint8_t rex = 0x48;
    if (reg1 >= R8) rex |= 0x04; 
    if (reg2 >= R8) rex |= 0x01;
    WriteByte(rex, bin_file);

    WriteByte(0x31, bin_file);  // 0x31 = opcode xor r1, r2
    WriteByte((uint8_t)(0xC0 | ((reg1 & 7) << 3) | (reg2 & 7)), bin_file);

    return 3;
}

// shl r, num
size_t WriteShlRNum(registers_64 reg, uint8_t num, FILE* bin_file) {
    uint8_t rex = 0x48;
    if (reg >= R8) rex |= 0x01;
    WriteByte(rex, bin_file);

    WriteByte(0xC1, bin_file); // 0xC1 = opcode of group instructions
    // Mod: 11         - 11 because we shl r, num, without memory (7-6 bits)
    // Reg: 100        - opcode extension 100 (4 = shl)
    // R/M: reg &  7   - reg -> [0, 7] (2-0 bits)
    WriteByte((uint8_t)(0xC0  | (4 << 3) | (reg & 7)), bin_file); 
    WriteByte(num, bin_file);

    return 4;
}

// neg r
size_t WriteNegR(registers_64 reg, FILE* bin_file) {
    uint8_t rex = 0x48;
    if (reg >= R8) rex |= 0x01;
    WriteByte(rex, bin_file);

    WriteByte(0xF7, bin_file);  // 0xF7 = opcode of group instructions
    // Mod: 11         - 11 because we neg r, without memory (7-6 bits)
    // Reg: 011        - opcode extension 011 (3 = neg)
    // R/M: reg & 7    - reg -> [0, 7] (2-0 bits)
    WriteByte((uint8_t)(0xC0 | (3 << 3) | (reg & 7)), bin_file);

    return 3;
}

// rol r, cl
size_t WriteRolRCl(registers_64 reg, FILE* bin_file) {
    uint8_t rex = 0x48;
    if (reg >= R8) rex |= 0x01;
    WriteByte(rex, bin_file);

    WriteByte(0xD3, bin_file); // 0xD2 = opcode of group instructions
    // Mod: 11         - 11 because we neg r, without memory (7-6 bits)
    // Reg: 000        - opcode extension 000 (0 = rol)
    // R/M: reg & 7    - reg -> [0, 7] (2-0 bits)
    WriteByte((uint8_t)(0xC0 | (reg & 7)), bin_file); 

    return 3;
}

// cvtsi2sd xmm, r
size_t WriteCvtsi2sd(xmm xmm_idx, registers_64 reg_idx, FILE* bin_file) {
    WriteByte(0xF2, bin_file);  // work with double

    uint8_t rex = 0x48; 
    if (reg_idx >= R8) rex |= 0x01;
    WriteByte(rex, bin_file);

    // 0x0F 0x2A = opcode cvtsi2sd
    WriteByte(0x0F, bin_file);
    WriteByte(0x2A, bin_file); 
    WriteByte((uint8_t)(0xC0 | ((xmm_idx & 7) << 3) | (reg_idx & 7)), bin_file);

    return 5;
}

// cvttsd2si r, xmm
size_t WriteCvttsd2si(registers_64 reg_idx, xmm xmm_idx, FILE* bin_file) {
    WriteByte(0xF2, bin_file);  // work with double

    uint8_t rex = 0x48; 
    if (reg_idx >= R8) rex |= 0x04;     
    WriteByte(rex, bin_file);
    
    // 0x0F 0x2C = opcode cvttsd2si
    WriteByte(0x0F, bin_file);
    WriteByte(0x2C, bin_file); 
    WriteByte((uint8_t)(0xC0 | ((reg_idx & 7) << 3) | (xmm_idx & 7)), bin_file);

    return 5;
}

// sqrtsd xmm1, xmm2
size_t WriteSqrtsd(xmm xmm1, xmm xmm2, FILE* bin_file) {
    WriteByte(0xF2, bin_file);  // work with double

    // 0x0F 0x51 = opcode sqrtsd
    WriteByte(0x0F, bin_file);
    WriteByte(0x51, bin_file); 
    WriteByte((uint8_t)(0xC0 | ((xmm1 & 7) << 3) | (xmm2 & 7)), bin_file);

    return 4;
}

// cqo
size_t WriteCqo(FILE* bin_file) {
    // 0x48 0x99 = opcode cqo
    WriteByte(0x48, bin_file); 
    WriteByte(0x99, bin_file); 

    return 2;
}

// j(code) offset
static size_t WriteJcc(uint8_t code, int32_t offset, FILE* bin_file) {
    // 0x0F code = opcode j(code)
    WriteByte(0x0F, bin_file);
    WriteByte(code, bin_file);
    WriteInt32(offset, bin_file);
    
    return 6;
}

size_t WriteJle(int32_t offset, FILE* bin_file)  { return WriteJcc(0x8E, offset, bin_file); }
size_t WriteJl(int32_t offset, FILE* bin_file)   { return WriteJcc(0x8C, offset, bin_file); }
size_t WriteJge(int32_t offset, FILE* bin_file)  { return WriteJcc(0x8D, offset, bin_file); }
size_t WriteJg(int32_t offset, FILE* bin_file)   { return WriteJcc(0x8F, offset, bin_file); }
size_t WriteJne(int32_t offset, FILE* bin_file)  { return WriteJcc(0x85, offset, bin_file); }
size_t WriteJe(int32_t offset, FILE* bin_file)   { return WriteJcc(0x84, offset, bin_file); }
size_t WriteJnz(int32_t offset, FILE* bin_file)  { return WriteJcc(0x85, offset, bin_file); }
size_t WriteJz(int32_t offset, FILE* bin_file)   { return WriteJcc(0x84, offset, bin_file); }
size_t WriteJns(int32_t offset, FILE* bin_file)  { return WriteJcc(0x89, offset, bin_file); }

size_t WriteJmp(int32_t offset, FILE* bin_file) {
    WriteByte(0xE9, bin_file);  // 0xE9 = opcode jmp
    WriteInt32(offset, bin_file);

    return 5;
}

size_t WriteJmpR(registers_64 reg, FILE* bin_file) {
    uint8_t rex = 0x48;
    if (reg >= R8) rex |= 0x01; 
    WriteByte(rex, bin_file);

    WriteByte(0xFF, bin_file);  // 0xFF = opcode of group instructions
    // Mod: 11         - 11 because we jmp r, without memory (7-6 bits)
    // Reg: 100        - opcode extension 100 (4 = jmp)
    // R/M: reg & 7    - reg -> [0, 7] (2-0 bits)
    WriteByte((uint8_t)(0xC0 | (4 << 3) | (reg & 7)), bin_file);

    return 3;
}

size_t WriteLoop(int8_t offset, FILE* bin_file) {
    // 0xE2 = opcode loop
    WriteByte(0xE2, bin_file);
    WriteByte((uint8_t)offset, bin_file);

    return 2;
}

size_t WriteRet(FILE* bin_file) {
    // 0xC3 = opcode ret
    WriteByte(0xC3, bin_file);

    return 1;
}

size_t WriteSyscall(FILE* bin_file) {
    // 0x0F 0x05 = opcode cqo
    WriteByte(0x0F, bin_file); 
    WriteByte(0x05, bin_file); 

    return 2;
}

size_t WriteCall(int32_t offset, FILE* bin_file) {
    // 0xE8 = opcode call
    WriteByte(0xE8, bin_file);
    WriteInt32(offset, bin_file);

    return 5;
}