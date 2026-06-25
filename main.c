#include <stdint.h>
#include <stdio.h>
#include <memory.h>

// Flags
#define FLAG_C 0x01
#define FLAG_Z 0x02
#define FLAG_I 0x04
#define FLAG_D 0x08
#define FLAG_B 0x10
#define FLAG_U 0x20
#define FLAG_V 0x40
#define FLAG_N 0x80

// Instructions
#define OP_BRK 0x00

#define OP_LDA_IMM 0xA9
#define OP_LDA_ABS 0xAD
#define OP_LDX_IMM 0xA2
#define OP_LDX_ABS 0xAE
#define OP_LDY_IMM 0xA0
#define OP_LDY_ABS 0xAC

#define OP_TAX 0xAA
#define OP_TAY 0xA8
#define OP_TYA 0x98
#define OP_TXA 0x8A
#define OP_TSX 0xBA
#define OP_TXS 0x9A

#define OP_INX 0xE8
#define OP_INY 0xC8
#define OP_DEX 0xCA
#define OP_DEY 0x88

#define OP_STX_ZP 0x86
#define OP_STX_ZP_Y 0x96
#define OP_STX_ABS 0x8E

uint8_t memory[65536];

typedef struct
{
    uint8_t A;
    uint8_t X;
    uint8_t Y;

    uint8_t SP;
    uint16_t PC;
    uint8_t P;
} CPU;

// Helper functions
void set_flag(CPU *cpu, uint8_t flag, uint8_t value)
{
    if (value)
        cpu->P |= flag;
    else
        cpu->P &= ~flag;
}

// Instruction set
void lda_imm(CPU *cpu)
{
    cpu->A = memory[cpu->PC++];

    set_flag(cpu, FLAG_Z, cpu->A == 0);
    set_flag(cpu, FLAG_N, cpu->A & 0x80);
}

void lda_abs(CPU *cpu)
{
    uint16_t addr = memory[cpu->PC] | (memory[cpu->PC + 1] << 8);

    cpu->A = memory[addr];
    cpu->PC += 2;

    set_flag(cpu, FLAG_Z, cpu->A == 0);
    set_flag(cpu, FLAG_N, cpu->A & 0x80);
}

void ldx_imm(CPU *cpu)
{
    cpu->X = memory[cpu->PC++];

    set_flag(cpu, FLAG_Z, cpu->X == 0);
    set_flag(cpu, FLAG_N, cpu->X & 0x80);
}

void ldx_abs(CPU *cpu)
{
    uint16_t addr = memory[cpu->PC] | (memory[cpu->PC + 1] << 8);

    cpu->X = memory[addr];
    cpu->PC += 2;

    set_flag(cpu, FLAG_Z, cpu->X == 0);
    set_flag(cpu, FLAG_N, cpu->X & 0x80);
}

void ldy_imm(CPU *cpu)
{
    cpu->Y = memory[cpu->PC++];

    set_flag(cpu, FLAG_Z, cpu->Y == 0);
    set_flag(cpu, FLAG_N, cpu->Y & 0x80);
}

void ldy_abs(CPU *cpu)
{
    uint16_t addr = memory[cpu->PC] | (memory[cpu->PC + 1] << 8);

    cpu->Y = memory[addr];
    cpu->PC += 2;

    set_flag(cpu, FLAG_Z, cpu->Y == 0);
    set_flag(cpu, FLAG_N, cpu->Y & 0x80);
}

void tax(CPU *cpu)
{
    cpu->X = cpu->A;

    set_flag(cpu, FLAG_Z, cpu->X == 0);
    set_flag(cpu, FLAG_N, cpu->X & 0x80);
}

void tay(CPU *cpu)
{
    cpu->Y = cpu->A;

    set_flag(cpu, FLAG_Z, cpu->Y == 0);
    set_flag(cpu, FLAG_N, cpu->Y & 0x80);
}

void tya(CPU *cpu)
{
    cpu->A = cpu->Y;

    set_flag(cpu, FLAG_Z, cpu->A == 0);
    set_flag(cpu, FLAG_N, cpu->A & 0x80);
}

void txa(CPU *cpu)
{
    cpu->A = cpu->X;

    set_flag(cpu, FLAG_Z, cpu->A == 0);
    set_flag(cpu, FLAG_N, cpu->A & 0x80);
}

void tsx(CPU *cpu)
{
    cpu->X = cpu->SP;

    set_flag(cpu, FLAG_Z, cpu->X == 0);
    set_flag(cpu, FLAG_N, cpu->X & 0x80);
}

void txs(CPU *cpu) // TXS Does not affect any flags
{
    cpu->SP = cpu->X;
}

void inx(CPU *cpu)
{
    cpu->X++;

    set_flag(cpu, FLAG_Z, cpu->X == 0);
    set_flag(cpu, FLAG_N, cpu->X & 0x80);
}

void iny(CPU *cpu)
{
    cpu->Y++;

    set_flag(cpu, FLAG_Z, cpu->Y == 0);
    set_flag(cpu, FLAG_N, cpu->Y & 0x80);
}

void dex(CPU *cpu)
{
    cpu->X--;

    set_flag(cpu, FLAG_Z, cpu->X == 0);
    set_flag(cpu, FLAG_N, cpu->X & 0x80);
}

void dey(CPU *cpu)
{
    cpu->Y--;

    set_flag(cpu, FLAG_Z, cpu->Y == 0);
    set_flag(cpu, FLAG_N, cpu->Y & 0x80);
}

void stx_zp(CPU *cpu)
{
    uint8_t addr = memory[cpu->PC++];
    memory[addr] = cpu->X;
}

void stx_zp_y(CPU *cpu)
{
    uint8_t addr = memory[cpu->PC++] + cpu->Y;
    memory[addr] = cpu->X;
}

void stx_abs(CPU *cpu)
{
    uint16_t addr = memory[cpu->PC] | (memory[cpu->PC + 1] << 8);

    memory[addr] = cpu->X;

    cpu->PC += 2;
}

int main()
{
    CPU cpu6502;
    memset(&cpu6502, 0, sizeof(cpu6502)); // Initialize CPU registers 0
    memset(&memory, 0, sizeof(memory));   // Initialize memory 0

    memory[0x1000] = 0x05;
    memory[0x1001] = 0x42;
    memory[0x1002] = 0x99;

    // ---------- Load registers ----------

    memory[0] = 0xA9; // LDA #$12
    memory[1] = 0x12;

    memory[2] = 0xAA; // TAX

    memory[3] = 0xA8; // TAY

    memory[4] = 0xE8; // INX
    memory[5] = 0xC8; // INY

    memory[6] = 0xCA; // DEX
    memory[7] = 0x88; // DEY

    memory[8] = 0x98; // TYA
    memory[9] = 0x8A; // TXA

    // ---------- Absolute loads ----------

    memory[10] = 0xAC; // LDY $1000
    memory[11] = 0x00;
    memory[12] = 0x10;

    memory[13] = 0xAE; // LDX $1001
    memory[14] = 0x01;
    memory[15] = 0x10;

    memory[16] = 0xAD; // LDA $1002
    memory[17] = 0x02;
    memory[18] = 0x10;

    // ---------- Stack transfers ----------

    memory[19] = 0x9A; // TXS
    memory[20] = 0xBA; // TSX

    // ---------- STX Zero Page ----------

    memory[21] = 0x86;
    memory[22] = 0x20;

    // ---------- STX Zero Page,Y ----------

    memory[23] = 0x96;
    memory[24] = 0x30;

    // ---------- STX Absolute ----------

    memory[25] = 0x8E;
    memory[26] = 0x00;
    memory[27] = 0x20;

    // ---------- BRK ----------

    memory[28] = 0x00;

    uint8_t opcode;
    int done = 0;
    while (!done)
    {
        opcode = memory[cpu6502.PC++];

        switch (opcode)
        {
        case OP_LDA_IMM:
            lda_imm(&cpu6502);
            break;
        case OP_LDA_ABS:
            lda_abs(&cpu6502);
            break;
        case OP_LDX_IMM:
            ldx_imm(&cpu6502);
            break;
        case OP_LDX_ABS:
            ldx_abs(&cpu6502);
            break;
        case OP_LDY_IMM:
            ldy_imm(&cpu6502);
            break;
        case OP_LDY_ABS:
            ldy_abs(&cpu6502);
            break;
        case OP_TAX:
            tax(&cpu6502);
            break;
        case OP_TAY:
            tay(&cpu6502);
            break;
        case OP_TYA:
            tya(&cpu6502);
            break;
        case OP_TXA:
            txa(&cpu6502);
            break;
        case OP_TSX:
            tsx(&cpu6502);
            break;
        case OP_TXS:
            txs(&cpu6502);
            break;
        case OP_INX:
            inx(&cpu6502);
            break;
        case OP_INY:
            iny(&cpu6502);
            break;
        case OP_DEX:
            dex(&cpu6502);
            break;
        case OP_DEY:
            dey(&cpu6502);
            break;
        case OP_STX_ZP:
            stx_zp(&cpu6502);
            break;
        case OP_STX_ZP_Y:
            stx_zp_y(&cpu6502);
            break;
        case OP_STX_ABS:
            stx_abs(&cpu6502);
            break;
        case OP_BRK:
            done = 1;
            break;
        default:
            printf("Unknown opcode: 0x%02X\n", opcode);
            done = 1;
            break;
        }
    }

    printf("Register A: 0x%02X\n", cpu6502.A);
    printf("Register X: 0x%02X\n", cpu6502.X);
    printf("Register Y: 0x%02X\n", cpu6502.Y);
    printf("Register SP: 0x%02X\n", cpu6502.SP);

    printf("\n");

    printf("memory[0x20]   = 0x%02X\n", memory[0x20]);
    printf("memory[0x35]   = 0x%02X\n", memory[0x35]);
    printf("memory[0x2000] = 0x%02X\n", memory[0x2000]);

    printf("PC: %d\n", cpu6502.PC);

    return 0;
}
