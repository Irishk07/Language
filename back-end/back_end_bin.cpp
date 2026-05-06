#include <assert.h>
#include <elf.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "back_end_bin.h"


void GenerateElf(FILE* bin_file) {
    assert(bin_file);

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

    const char* f_out = "Answer = %d\n";
    const char* f_in  = "%lld";
    const char* hex   = "0123456789ABCDEF";
    float f10 = 10.0f;
    uint32_t f_abs = 0x7FFFFFFF;

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

void WriteJumpTable(Labels* labels, FILE* bin_file) {
    assert(bin_file);

    fseek(bin_file, ADDR_JMP_TABLE - ADDR_BASE, SEEK_SET);

    uint64_t addr_b       = GetLabelAddress("mprintf_case_b", labels);
    uint64_t addr_c       = GetLabelAddress("mprintf_case_c", labels);
    uint64_t addr_d       = GetLabelAddress("mprintf_case_d", labels);
    uint64_t addr_f       = GetLabelAddress("mprintf_case_f", labels);
    uint64_t addr_o       = GetLabelAddress("mprintf_case_o", labels);
    uint64_t addr_s       = GetLabelAddress("mprintf_case_s", labels);
    uint64_t addr_x       = GetLabelAddress("mprintf_case_x", labels);
    uint64_t addr_default = GetLabelAddress("mprintf_case_default", labels);

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

#define WRITE_BYTE(byte) size += WriteByte((uint8_t)(byte), is_second_pass, bin_file);
#define WRITE_INT32(num) size += WriteInt32(num, is_second_pass, bin_file);

#define MODRM(Mod, Reg, RM) (Mod << 6) | ((Reg) << 3) | (RM)

static size_t WriteByte(uint8_t byte, bool is_second_pass, FILE* bin_file) { 
    if (!bin_file || !is_second_pass)
        return 0;

    fwrite(&byte, 1, 1, bin_file);

    return 1;
}

static size_t WriteInt32(int32_t num, bool is_second_pass, FILE* bin_file) {
    if (!bin_file || !is_second_pass)
        return 0;

    size_t size = 0;

    // Little-endian:
    WRITE_BYTE((uint32_t)num & 0xFF)          // last 8 bits
    WRITE_BYTE(((uint32_t)num >> 8) & 0xFF)
    WRITE_BYTE(((uint32_t)num >> 16) & 0xFF)
    WRITE_BYTE(((uint32_t)num >> 24) & 0xFF)

    return size;
}

uint64_t GetLabelAddress(const char* name, Labels* labels) {
    assert(name);

    for (size_t i = 0; i < labels->labels_count; ++i) {
        if (strcmp(labels->labels_table[i].name, name) == 0)
            return labels->labels_table[i].address;
    }

    return 0;
}

void AddLabel(const char* name, Labels* labels, uint64_t address) {
    assert(name);

    if (labels->labels_count < MAX_CNT_LABELS) {
        strcpy(labels->labels_table[labels->labels_count].name, name);
        labels->labels_table[labels->labels_count].address = address;
        labels->labels_count++;
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
size_t WritePushR(registers_64 reg, bool is_second_pass, FILE* bin_file) {
    size_t size = 0;
    if (reg >= R8)
        WRITE_BYTE(0x41)  // REX prefix
        
    WRITE_BYTE(0x50 + (reg & 7)); // 0x50 = opcode push; reg_id & 7: reg_id -> [0; 7];
    
    return size;
}

// push num
size_t WritePushNum(int32_t num, bool is_second_pass, FILE* bin_file) {
    size_t size = 0;

    if (num >= -128 && num <= 127) {   // if one signed byte
        WRITE_BYTE(0x6A)          // 0x6A = opcode push imm8
        WRITE_BYTE(num)
    } else {
        WRITE_BYTE(0x68)          // 0x68 = opcode push imm32
        WRITE_INT32(num)
    }

    return size;
}

// push [r]
size_t WritePushM(registers_64 reg, bool is_second_pass, FILE* bin_file) {
    size_t size = 0;
    if (reg >= R8)
        WRITE_BYTE(0x41) // REX prefix

    WRITE_BYTE(0xFF)      // opcode of group instrctions which use memory (push, inc, dec), Reg = 6 if push

    // byte ModR/M: [Mod: 2 bits] [Reg: 3 bits] [R/M: 3 bits]
    // Mod: 0x00 << 6  - 0x00 becaue we use [reg] without offset (7-6 bits)
    // Reg: 6 << 3     - 6 for 0xFF because push (5-3 bits)
    // R/M: reg_id & 7 - reg_id -> [0, 7] (2-0 bits)
    WRITE_BYTE(MODRM(0, 6, reg & 7))

    // if reg_id & 7 == 4 => proc wait SIB (Scale-Index-Base) byte (r12/rsp)
    // for example mov rax, [rbx + rcx * 4 + 0x10]
    if ((reg & 7) == 4)
        WRITE_BYTE(0x24) // 0x24 = SIB: base = [reg], no index

    // if reg_id & 7 == 5 && mod == 00 => proc think that we use absolute address (rip-relative) (r13/rbp)
    // for example mox rax, [rel buf]
    else if ((reg & 7) == 5) {
        // Change Mov: 01 - offset 1 byte
        if (bin_file && is_second_pass)
            fseek(bin_file, -1, SEEK_CUR); 
        size--;
        WRITE_BYTE(MODRM(1, 6, reg & 7))
        
        // 00 = offset (we have [r13 + 0]/[rbp + 0])
        WRITE_BYTE(0x00)
    }

    return size;
}

// pop r
// work such as push r
size_t WritePopR(registers_64 reg, bool is_second_pass, FILE* bin_file) {
    size_t size = 0;
    if (reg >= R8)
        WRITE_BYTE(0x41)

    WRITE_BYTE(0x58 + (reg & 7))   // 0x58 = opcode pop

    return size;
}

// pop [r]
size_t WritePopM(registers_64 reg, bool is_second_pass, FILE* bin_file) {
    size_t size = 0;
    if (reg >= R8)
        WRITE_BYTE(0x41) // REX prefix

    WRITE_BYTE(0x8F)     // 0x8F = opcode of group instructions 

    // byte ModR/M: [Mod: 2 bits] [Reg: 3 bits] [R/M: 3 bits]
    // Mod: 0x00 << 6  - 0x00 because we use [reg] without offset (7-6 bits)
    // Reg: 0 << 3     - 0 for 0x8F because pop (5-3 bits)
    // R/M: reg_id & 7 - reg_id -> [0, 7] (2-0 bits)
    WRITE_BYTE(MODRM(0, 0, reg & 7))

    // such as push [r]
    if ((reg & 7) == 4)
        WRITE_BYTE(0x24)
    else if ((reg & 7) == 5) {
        if (bin_file && is_second_pass)
            fseek(bin_file, -1, SEEK_CUR);
        size--;
        WRITE_BYTE(MODRM(1, 0, reg & 7))
        
        WRITE_BYTE(0x00)
    }

    return size;
}

// mov r1, r2
size_t WriteMovR1R2(registers_64 reg1, registers_64 reg2, bool is_second_pass, FILE* bin_file) {
    size_t size = 0;
    uint8_t rex = 0x48;          // use 64-bit regs
    if (reg2 >= R8) rex |= 0x04; // REX.R (+8 reg2)
    if (reg1 >= R8) rex |= 0x01; // REX.B (+8 reg1)

    WRITE_BYTE(rex)
    WRITE_BYTE(0x89)   // 0x89 = opcode mov r1, r2

    // Mod: 0xC0 (11)  - 0xC0 because we mov r1, r2, without memory (7-6 bits)
    // Reg: reg2 & 7   - reg2 -> [0, 7] (5-3 bits)
    // R/M: reg1 & 7   - reg1 -> [0, 7] (2-0 bits)
    WRITE_BYTE(MODRM(3, reg2 & 7, reg1 & 7))

    return size;
}

// mov [r1], r2
size_t WriteMovMrR(registers_64 reg1, registers_64 reg2, bool is_second_pass, FILE* bin_file) {
    size_t size = 0;
    uint8_t rex = 0x48;          // use 64-bit regs
    if (reg2 >= R8) rex |= 0x04; // REX.R
    if (reg1 >= R8) rex |= 0x01; // REX.B

    WRITE_BYTE(rex)
    WRITE_BYTE(0x89)   // 0x89 = opcode mov [r1], r2

    // if dst R13/RBP (rip-relative)
    if ((reg1 & 7) == 5) {
        // Mod: 0x00       - 0x01 because we mov [r1 + 0], r2, with offset 1 byte (7-6 bits)
        // Reg: reg2 & 7   - reg2 -> [0, 7] (5-3 bits)
        // R/M: reg1 & 7   - reg1 -> [0, 7] (2-0 bits)
        WRITE_BYTE(MODRM(1, reg2 & 7, reg1 & 7))
        WRITE_BYTE(0x00) // offset 0

        return size;
    }

    // Mod: 0x00 (00)  - 0x00 because we mov [r1], r2, without offset (7-6 bits)
    // Reg: reg2 & 7   - reg2 -> [0, 7] (5-3 bits)
    // R/M: reg1 & 7   - reg1 -> [0, 7] (2-0 bits)
    WRITE_BYTE(MODRM(0, reg2 & 7, reg1 & 7))
    
    // if dst R12/RSP (SIB)
    if ((reg1 & 7) == 4) {
        WRITE_BYTE(0x24)

        return size;
    }

    return size;
}

// mov [r1], r2
// r2 8-bit
size_t WriteMovMrR8(registers_64 reg1, registers_8 reg2, bool is_second_pass, FILE* bin_file) {
    size_t size = 0;
    uint8_t rex = 0x40;         // because src 8-bit
    if (reg1 >= R8) {
        rex |= 0x01;
        WRITE_BYTE(rex)
    }

    WRITE_BYTE(0x88)  // 0x88 = opcode mov [r1], r2 where r2 8-bit

    // if dst R13/RBP (rip-relative)
    if ((reg1 & 7) == 5) {
        // Mod: 0x01       - 0x01 because we mov [r1 + 0], r2, with offset 1 byte (7-6 bits)
        // Reg: reg2 & 7   - reg2 -> [0, 7] (5-3 bits)
        // R/M: reg1 & 7   - reg1 -> [0, 7] (2-0 bits)
        WRITE_BYTE(MODRM(1, reg2 & 7, reg1 & 7));
        WRITE_BYTE(0x00)

        return size;
    }

    // Mod: 0x00 (00)  - 0x00 because we mov [r1], r2, without offset (7-6 bits)
    // Reg: reg2 & 7   - reg2 -> [0, 7] (5-3 bits)
    // R/M: reg1 & 7   - reg1 -> [0, 7] (2-0 bits)
    WRITE_BYTE(MODRM(0, reg2 & 7, reg1 & 7));

    // if dst R12/RSP (SIB)
    if ((reg1 & 7) == 4)
        WRITE_BYTE(0x24)

    return size;
}

// mov r1, [r2]
size_t WriteMovRMr(registers_64 reg1, registers_64 reg2, bool is_second_pass, FILE* bin_file) {
    size_t size = 0;
    uint8_t rex = 0x48;             // use 64-bit regs
    if (reg1 >= R8) rex |= 0x04; 
    if (reg2 >= R8) rex |= 0x01;
    WRITE_BYTE(rex)

    WRITE_BYTE(0x8B)     // 0x8B = opcode mov r1, [r2]

    // if src R13/RBP (rip-relative)
    if ((reg2 & 7) == 5) {
        // Mod: 0x01       - 0x01 because we mov [r1 + 0], r2, with offset 1 byte (7-6 bits)
        // Reg: reg2 & 7   - reg2 -> [0, 7] (5-3 bits)
        // R/M: reg1 & 7   - reg1 -> [0, 7] (2-0 bits)
        WRITE_BYTE(MODRM(1, reg1 & 7, reg2 & 7))
        WRITE_BYTE(0x00); // offset 0
        
        return size; 
    }

    // Mod: 0x00 (00)  - 0x00 because we mov r1, [r2], without offset (7-6 bits)
    // Reg: reg2 & 7   - reg2 -> [0, 7] (5-3 bits)
    // R/M: reg1 & 7   - reg1 -> [0, 7] (2-0 bits)
    WRITE_BYTE(MODRM(0, reg1 & 7, reg2 & 7))

    // if src R12/RSP (SIB)
    if ((reg2 & 7) == 4)
        WRITE_BYTE(0x24)

    return size;
}

// mov r1, [r2]
// r1 8-bit
size_t WriteMovR8Mr(registers_8 reg1, registers_64 reg2, bool is_second_pass, FILE* bin_file) {
    size_t size = 0;
    uint8_t rex = 0x40;         // because dst 8-bit
    if (reg2 >= R8) {
        rex |= 0x01;
        WRITE_BYTE(rex)
    }

    WRITE_BYTE(0x8A)  // 0x8A = opcode mov r1, [r2] where r1 8-bit

    // if src R13/RBP (rip-relative)
    if ((reg2 & 7) == 5) {
        WRITE_BYTE(MODRM(1, reg1 & 7, reg2 & 7))
        WRITE_BYTE(0x00)

        return size;
    }

    WRITE_BYTE(MODRM(0, reg1 & 7, reg2 & 7))

    // if src R12/RSP (SIB)
    if ((reg2 & 7) == 4)
        WRITE_BYTE(0x24)

    return size;
}

// lea r, [addr]
size_t WriteMovRAddr(registers_64 reg, uint64_t addr, bool is_second_pass, FILE* bin_file) {
    size_t size = 0;
    uint8_t rex = 0x48;
    if (reg >= R8) rex |= 0x01; 
    
    WRITE_BYTE(rex)
    WRITE_BYTE(0xB8 + (reg & 7))     // 0xB8 opcode mov r, addr

    if (bin_file && is_second_pass) { 
        fwrite(&addr, 8, 1, bin_file);
        size += 8;
    }    

    return size;
}

// mor r, num
size_t WriteMovRNum(registers_64 reg, int64_t num, bool is_second_pass, FILE* bin_file) {
    size_t size = 0;
    uint8_t rex = 0x48;
    if (reg >= R8) rex |= 0x01;

    WRITE_BYTE(rex)
    WRITE_BYTE(0xB8 + (reg & 7))          // 0xB8 opcode mov r, num

    if (bin_file && is_second_pass) {
        fwrite(&num, 8, 1, bin_file); 
        size += 8;
    }

    return size;
}

// mov r, char
// r 8-bit
size_t WriteMovR8Char(registers_8 reg, char symb, bool is_second_pass, FILE* bin_file) {
    size_t size = 0;
    WRITE_BYTE((0xB0 + (reg & 7))) // 0xB0 opcode mov r, char
    WRITE_BYTE(symb)

    return size;
}

// movq xmm, [r]
size_t WriteMovqXmmMr(xmm xmm_id, registers_64 reg, bool is_second_pass, FILE* bin_file) {
    size_t size = 0;
    WRITE_BYTE(0x66)      // 0x66 - we work with xmm

    uint8_t rex = 0x48; 
    if (reg >= R8) rex |= 0x01;
    WRITE_BYTE(rex)

    // 0x0F 0x7E - opcode of movq
    WRITE_BYTE(0x0F)
    WRITE_BYTE(0x7E)

    WRITE_BYTE(MODRM(0, xmm_id & 7, reg & 7));

    return size;
}

// movzx r1, r2
// r2 8-bit
size_t WriteMovzxR1R2(registers_64 reg1, registers_8 reg2, bool is_second_pass, FILE* bin_file) {
    size_t size = 0;
    uint8_t rex = 0x48;         // because result 8-bit
    if (reg1 >= R8) {
        rex |= 0x04;
        WRITE_BYTE(rex)
    }

    // 0x0F 0xB6 = opcode movzx r1, r2 where r2 8-bit
    WRITE_BYTE(0x0F)
    WRITE_BYTE(0xB6)

    WRITE_BYTE(MODRM(3, reg1 & 7, reg2 & 7))

    return size;
}

// movzx r1, byte [r2]
size_t WriteMovzxRMr(registers_64 reg1, registers_64 reg2, bool is_second_pass, FILE* bin_file) {
    size_t size = 0;
    uint8_t rex = 0x48;
    if (reg1 >= R8) rex |= 0x04;
    if (reg2 >= R8) rex |= 0x01;
    WRITE_BYTE(rex)

    // 0x0F 0xB6 = opcode movzx r1, byte [r2]
    WRITE_BYTE(0x0F)
    WRITE_BYTE(0xB6)

    WRITE_BYTE(MODRM(0, reg1 & 7, reg2 & 7))

    return size;
}

// movsxd r1, r2
// r2 32-bit
size_t WriteMovsxdR1R2(registers_64 reg1, registers_32 reg2, bool is_second_pass, FILE* bin_file) {
    size_t size = 0;
    uint8_t rex = 0x48;
    if (reg1 >= R8) rex |= 0x04;
    WRITE_BYTE(rex)

    WRITE_BYTE(0x63)  // 0x63 = opcode movsxd r1, r2 where r2 32-bit

    WRITE_BYTE(MODRM(3, reg1 & 7, reg2 & 7))

    return size;
}

// sub r1, r2
size_t WriteSubR1R2(registers_64 reg1, registers_64 reg2, bool is_second_pass, FILE* bin_file) {
    size_t size = 0;
    uint8_t rex = 0x48;
    if (reg2 >= R8) rex |= 0x04; 
    if (reg1 >= R8) rex |= 0x01; 
    WRITE_BYTE(rex)

    WRITE_BYTE(0x29) // 0x29 = opcode sub r1, r2

    WRITE_BYTE(MODRM(3, reg2 & 7, reg1 & 7))

    return size;
}

// sub r, num
size_t WriteSubRNum(registers_64 reg, int32_t num, bool is_second_pass, FILE* bin_file) {
    size_t size = 0;
    uint8_t rex = 0x48;
    if (reg >= R8) rex |= 0x01; 
    WRITE_BYTE(rex)

    if (num >= -128 && num <= 127) {    // 1 byte
        WRITE_BYTE(0x83)      // 0x83 = opcode of short form
        // Mod: 11         - 11 because we sub r1, num, without memory (7-6 bits)
        // Reg: 101        - opcode extension 101 (5 = sub)
        // R/M: reg &  7   - reg -> [0, 7] (2-0 bits)
        WRITE_BYTE((0xC0 | (5 << 3) | (reg & 7))) 
        WRITE_BYTE(num)
    } else {
        WRITE_BYTE(0x81)      // 0x81 = opcode of long form
        WRITE_BYTE((0xC0 | (5 << 3) | (reg & 7)));
        WRITE_INT32(num)
    }

    return size;
}

// dec r
size_t WriteDecR(registers_64 reg, bool is_second_pass, FILE* bin_file) {
    size_t size = 0;
    uint8_t rex = 0x48;
    if (reg >= R8) rex |= 0x01;
    WRITE_BYTE(rex)

    WRITE_BYTE(0xFF) // opcode of group instructions

    // Mod: 11         - 11 because we dec r without memory (7-6 bits)
    // Reg: 001        - opcode extension 001 (1 = dec)
    // R/M: reg & 7   - reg -> [0, 7] (2-0 bits)
    WRITE_BYTE(MODRM(3, 1, reg & 7));

    return size;
}

// add r1, r2
size_t WriteAddR1R2(registers_64 reg1, registers_64 reg2, bool is_second_pass, FILE* bin_file) {
    size_t size = 0;
    uint8_t rex = 0x48;
    if (reg2 >= R8) rex |= 0x04; 
    if (reg1 >= R8) rex |= 0x01; 
    WRITE_BYTE(rex)

    WRITE_BYTE(0x01) // 0x01 opcode add r1, r2

    WRITE_BYTE(MODRM(3, reg2 & 7, reg1 & 7))

    return size;
}

size_t WriteAddRNum(registers_64 reg, int32_t num, bool is_second_pass, FILE* bin_file) {
    size_t size = 0;
    uint8_t rex = 0x48;
    if (reg >= R8) rex |= 0x01; 
    WRITE_BYTE(rex)

    if (num >= -128 && num <= 127) {    // 1 byte
        WRITE_BYTE(0x83)      // 0x83 = opcode of short form
        // Mod: 11         - 11 because we add r, num, without memory (7-6 bits)
        // Reg: 000        - opcode extension 000 (0 = add)
        // R/M: reg &  7   - reg -> [0, 7] (2-0 bits)
        WRITE_BYTE(MODRM(3, 0, reg & 7))
        WRITE_BYTE(num);
    } else {
        WRITE_BYTE(0x81)      // 0x81 = opcode of long form
        WRITE_BYTE(MODRM(3, 0, reg & 7))
        WRITE_INT32(num)
    }

    return size;
}

// inc r
size_t WriteIncR(registers_64 reg, bool is_second_pass, FILE* bin_file) {
    size_t size = 0;
    uint8_t rex = 0x48;
    if (reg >= R8) rex |= 0x01;
    WRITE_BYTE(rex)

    WRITE_BYTE(0xFF) // opcode of group instructions

    // Mod: 11         - 11 because we inc r without memory (7-6 bits)
    // Reg: 000        - opcode extension 000 (0 = inc)
    // R/M: reg & 7    - reg -> [0, 7] (2-0 bits)
    WRITE_BYTE(MODRM(3, 0, reg & 7))

    return size;
}

// div r
size_t WriteDivR(registers_64 reg, bool is_second_pass, FILE* bin_file) {
    size_t size = 0;
    uint8_t rex = 0x48; 
    if (reg >= R8) rex |= 0x01;
    WRITE_BYTE(rex)

    WRITE_BYTE(0xF7)  // 0xF7 = opcode div/idiv

    // Mod: 11         - 11 because we div r without memory (7-6 bits)
    // Reg: 110        - opcode extension 110 (6 = div)
    // R/M: reg & 7    - reg -> [0, 7] (2-0 bits)
    WRITE_BYTE(MODRM(3, 6, reg & 7))

    return size;
}

// idiv r
size_t WriteIdivR(registers_64 reg, bool is_second_pass, FILE* bin_file) {
    size_t size = 0;
    uint8_t rex = 0x48;
    if (reg >= R8) rex |= 0x01;
    WRITE_BYTE(rex)

    WRITE_BYTE(0xF7)   // 0xF7 = opcode div/idiv

    // Mod: 11         - 11 because we div r without memory (7-6 bits)
    // Reg: 111        - opcode extension 111 (7 = idiv)
    // R/M: reg & 7    - reg -> [0, 7] (2-0 bits)
    WRITE_BYTE(MODRM(3, 7, reg & 7));

    return size;
}

// imul r, r, num
size_t WriteImulRNum(registers_64 reg, int32_t num, bool is_second_pass, FILE* bin_file) {
    size_t size = 0;
    uint8_t rex = 0x48;
    if (reg >= R8) {
        rex |= 0x04; 
        rex |= 0x01;
    }
    WRITE_BYTE(rex)

    if (num >= -128 && num <= 127) {    // 1 byte
        WRITE_BYTE(0x6B)      // 0x6B = opcode short imul

        WRITE_BYTE(MODRM(3, reg & 7, reg & 7))
        WRITE_BYTE(num);
    } else {
        WRITE_BYTE(0x69)      // 0x69 = opcode long imul
        WRITE_BYTE(MODRM(3, reg & 7, reg & 7))
        WRITE_INT32(num)
    }

    return size;
}

// imul r1, r2
size_t WriteImulR1R2(registers_64 reg1, registers_64 reg2, bool is_second_pass, FILE* bin_file) {
    size_t size = 0;
    uint8_t rex = 0x48; 
    if (reg1 >= R8) rex |= 0x04; 
    if (reg2 >= R8) rex |= 0x01; 
    WRITE_BYTE(rex)

    WRITE_BYTE(0x0F)
    WRITE_BYTE(0xAF)
    
    WRITE_BYTE(MODRM(3, reg1 & 7, reg2 & 7))

    return size; 
}

// cmp r1, r2
size_t WriteCmpR1R2(registers_64 reg1, registers_64 reg2, bool is_second_pass, FILE* bin_file) {
    size_t size = 0;
    uint8_t rex = 0x48; 
    if (reg2 >= R8) rex |= 0x04; 
    if (reg1 >= R8) rex |= 0x01; 
    WRITE_BYTE(rex)

    WRITE_BYTE(0x39)  // 0x39 = opcode of cmp r1, r2
    WRITE_BYTE(MODRM(3, reg2 & 7, reg1 & 7)) 

    return size;
}

// cmp r, num
size_t WriteCmpRNum(registers_64 reg, int32_t num, bool is_second_pass, FILE* bin_file) {
    size_t size = 0;
    uint8_t rex = 0x48;
    if (reg >= R8) rex |= 0x01;
    WRITE_BYTE(rex)

    WRITE_BYTE(0x81)  // 0x81 = opcode of group instructions

    // Mod: 11         - 11 because we cmp r, num without memory (7-6 bits)
    // Reg: 111        - opcode extension 111 (7 = cmp)
    // R/M: reg & 7    - reg -> [0, 7] (2-0 bits)
    WRITE_BYTE(MODRM(3, 7, reg & 7))
    WRITE_INT32(num)

    return size;
}

// cmp r, char
// r 8-bit
size_t WriteCmpR8Char(registers_8 reg, uint8_t num, bool is_second_pass, FILE* bin_file) {
    size_t size = 0;
    WRITE_BYTE(0x80)  // 0x80 = opcode of group instructions (with 8-bit)

    // Mod: 11         - 11 because we cmp r, num without memory (7-6 bits)
    // Reg: 111        - opcode extension 111 (7 = cmp)
    // R/M: reg & 7    - reg -> [0, 7] (2-0 bits)
    WRITE_BYTE(MODRM(3, 7, reg & 7))
    WRITE_BYTE(num)

    return size;
}

// test r1, r2
size_t WriteTestR1R2(registers_64 reg1, registers_64 reg2, bool is_second_pass, FILE* bin_file) {
    size_t size = 0;
    uint8_t rex = 0x48;
    if (reg1 >= R8) rex |= 0x04;
    if (reg2 >= R8) rex |= 0x01;
    WRITE_BYTE(rex)

    WRITE_BYTE(0x85)  // 0x85 = opcode test r1, r1
    WRITE_BYTE(MODRM(3, reg1 & 7, reg2))

    return size;
}

// and r1, r2
size_t WriteAndR1R2(registers_64 reg1, registers_64 reg2, bool is_second_pass, FILE* bin_file) {
    size_t size = 0;
    uint8_t rex = 0x48;
    if (reg1 >= R8) rex |= 0x04;
    if (reg2 >= R8) rex |= 0x01;
    WRITE_BYTE(rex)

    WRITE_BYTE(0x21)  // 0x21 = opcode test r1, r1
    WRITE_BYTE(MODRM(3, reg1 & 7, reg2 & 7))

    return size;
}

// and r, num
size_t WriteAndRNum(registers_64 reg, int32_t num, bool is_second_pass, FILE* bin_file) {
    size_t size = 0;
    uint8_t rex = 0x48;
    if (reg >= R8) rex |= 0x01;
    WRITE_BYTE(rex)

    WRITE_BYTE(0x81) // 0x81 = opcode of group instructions
    // Mod: 11         - 11 because we and r, num, without memory (7-6 bits)
    // Reg: 100        - opcode extension 100 (4 = and)
    // R/M: reg &  7   - reg -> [0, 7] (2-0 bits)
    WRITE_BYTE(MODRM(3, 4, reg & 7))
    
    WRITE_INT32(num)

    return size;
}

// xor r1, r2
size_t WriteXorR1R2(registers_64 reg1, registers_64 reg2, bool is_second_pass, FILE* bin_file) {
    size_t size = 0;
    uint8_t rex = 0x48;
    if (reg1 >= R8) rex |= 0x04; 
    if (reg2 >= R8) rex |= 0x01;
    WRITE_BYTE(rex)

    WRITE_BYTE(0x31)  // 0x31 = opcode xor r1, r2
    WRITE_BYTE(MODRM(3, reg1 & 7, reg2 & 7))

    return size;
}

// shl r, num
size_t WriteShlRNum(registers_64 reg, uint8_t num, bool is_second_pass, FILE* bin_file) {
    size_t size = 0;
    uint8_t rex = 0x48;
    if (reg >= R8) rex |= 0x01;
    WRITE_BYTE(rex)

    WRITE_BYTE(0xC1) // 0xC1 = opcode of group instructions
    // Mod: 11         - 11 because we shl r, num, without memory (7-6 bits)
    // Reg: 100        - opcode extension 100 (4 = shl)
    // R/M: reg &  7   - reg -> [0, 7] (2-0 bits)
    WRITE_BYTE(MODRM(3, 4, reg & 7)) 
    WRITE_BYTE(num)

    return size;
}

// neg r
size_t WriteNegR(registers_64 reg, bool is_second_pass, FILE* bin_file) {
    size_t size = 0;
    uint8_t rex = 0x48;
    if (reg >= R8) rex |= 0x01;
    WRITE_BYTE(rex)

    WRITE_BYTE(0xF7)  // 0xF7 = opcode of group instructions
    // Mod: 11         - 11 because we neg r, without memory (7-6 bits)
    // Reg: 011        - opcode extension 011 (3 = neg)
    // R/M: reg & 7    - reg -> [0, 7] (2-0 bits)
    WRITE_BYTE(MODRM(3, 3, reg & 7))

    return size;
}

// rol r, cl
size_t WriteRolRCl(registers_64 reg, bool is_second_pass, FILE* bin_file) {
    size_t size = 0;
    uint8_t rex = 0x48;
    if (reg >= R8) rex |= 0x01;
    WRITE_BYTE(rex)

    WRITE_BYTE(0xD3) // 0xD2 = opcode of group instructions
    // Mod: 11         - 11 because we neg r, without memory (7-6 bits)
    // Reg: 000        - opcode extension 000 (0 = rol)
    // R/M: reg & 7    - reg -> [0, 7] (2-0 bits)
    WRITE_BYTE(MODRM(3, 0, reg & 7))

    return size;
}

// cvtsi2sd xmm, r
size_t WriteCvtsi2sd(xmm xmm_id, registers_64 reg, bool is_second_pass, FILE* bin_file) {
    size_t size = 0;
    WRITE_BYTE(0xF2)  // work with double

    uint8_t rex = 0x48; 
    if (reg >= R8) rex |= 0x01;
    WRITE_BYTE(rex)

    // 0x0F 0x2A = opcode cvtsi2sd
    WRITE_BYTE(0x0F)
    WRITE_BYTE(0x2A) 
    WRITE_BYTE(MODRM(3, xmm_id & 7, reg & 7))

    return size;
}

// cvttsd2si r, xmm
size_t WriteCvttsd2si(registers_64 reg, xmm xmm_id, bool is_second_pass, FILE* bin_file) {
    size_t size = 0;
    WRITE_BYTE(0xF2)  // work with double

    uint8_t rex = 0x48; 
    if (reg >= R8) rex |= 0x04;     
    WRITE_BYTE(rex)
    
    // 0x0F 0x2C = opcode cvttsd2si
    WRITE_BYTE(0x0F)
    WRITE_BYTE(0x2C) 
    WRITE_BYTE(MODRM(3, reg & 7, xmm_id & 7))

    return size;
}

// sqrtsd xmm1, xmm2
size_t WriteSqrtsd(xmm xmm1, xmm xmm2, bool is_second_pass, FILE* bin_file) {
    size_t size = 0;
    WRITE_BYTE(0xF2)  // work with double

    // 0x0F 0x51 = opcode sqrtsd
    WRITE_BYTE(0x0F)
    WRITE_BYTE(0x51) 
    WRITE_BYTE(MODRM(3, xmm1 & 7, xmm2 & 7))

    return size;
}

// cqo
size_t WriteCqo(bool is_second_pass, FILE* bin_file) {
    size_t size = 0;
    // 0x48 0x99 = opcode cqo
    WRITE_BYTE(0x48) 
    WRITE_BYTE(0x99) 

    return size;
}

// j(code) offset
static size_t WriteJcc(uint8_t code, int32_t offset, bool is_second_pass, FILE* bin_file) {
    size_t size = 0;
    // 0x0F code = opcode j(code)
    WRITE_BYTE(0x0F)
    WRITE_BYTE(code)
    WRITE_INT32(offset)
    
    return size;
}

size_t WriteJle(int32_t offset, bool is_second_pass, FILE* bin_file)  { return WriteJcc(0x8E, offset, is_second_pass, bin_file); }
size_t WriteJl(int32_t offset, bool is_second_pass, FILE* bin_file)   { return WriteJcc(0x8C, offset, is_second_pass, bin_file); }
size_t WriteJge(int32_t offset, bool is_second_pass, FILE* bin_file)  { return WriteJcc(0x8D, offset, is_second_pass, bin_file); }
size_t WriteJg(int32_t offset, bool is_second_pass, FILE* bin_file)   { return WriteJcc(0x8F, offset, is_second_pass, bin_file); }
size_t WriteJne(int32_t offset, bool is_second_pass, FILE* bin_file)  { return WriteJcc(0x85, offset, is_second_pass, bin_file); }
size_t WriteJe(int32_t offset, bool is_second_pass, FILE* bin_file)   { return WriteJcc(0x84, offset, is_second_pass, bin_file); }
size_t WriteJnz(int32_t offset, bool is_second_pass, FILE* bin_file)  { return WriteJcc(0x85, offset, is_second_pass, bin_file); }
size_t WriteJz(int32_t offset, bool is_second_pass, FILE* bin_file)   { return WriteJcc(0x84, offset, is_second_pass, bin_file); }
size_t WriteJns(int32_t offset, bool is_second_pass, FILE* bin_file)  { return WriteJcc(0x89, offset, is_second_pass, bin_file); }

size_t WriteJmp(int32_t offset, bool is_second_pass, FILE* bin_file) {
    size_t size = 0;
    WRITE_BYTE(0xE9)  // 0xE9 = opcode jmp
    WRITE_INT32(offset)

    return size;
}

size_t WriteJmpR(registers_64 reg, bool is_second_pass, FILE* bin_file) {
    size_t size = 0;
    uint8_t rex = 0x48;
    if (reg >= R8) rex |= 0x01; 
    WRITE_BYTE(rex)

    WRITE_BYTE(0xFF)  // 0xFF = opcode of group instructions
    // Mod: 11         - 11 because we jmp r, without memory (7-6 bits)
    // Reg: 100        - opcode extension 100 (4 = jmp)
    // R/M: reg & 7    - reg -> [0, 7] (2-0 bits)
    WRITE_BYTE(MODRM(3, 4, reg & 7))

    return size;
}

size_t WriteLoop(int8_t offset, bool is_second_pass, FILE* bin_file) {
    size_t size = 0;
    WRITE_BYTE(0xE2)  // 0xE2 = opcode loop
    WRITE_BYTE(offset)

    return size;
}

size_t WriteRet(bool is_second_pass, FILE* bin_file) {
    size_t size = 0;
    WRITE_BYTE(0xC3)  // 0xC3 = opcode ret

    return size;
}

size_t WriteSyscall(bool is_second_pass, FILE* bin_file) {
    size_t size = 0;
    // 0x0F 0x05 = opcode cqo
    WRITE_BYTE(0x0F) 
    WRITE_BYTE(0x05) 

    return 2;
}

size_t WriteCall(int32_t offset, bool is_second_pass, FILE* bin_file) {
    size_t size = 0;
    // 0xE8 = opcode call
    WRITE_BYTE(0xE8)
    WRITE_INT32(offset)

    return size;
}

#undef WRITE_BYTE
#undef WRITE_INT32