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
void lda_immediate(CPU *cpu)
{
    cpu->A = memory[cpu->PC++];

    set_flag(cpu, FLAG_Z, cpu->A == 0);
    set_flag(cpu, FLAG_N, cpu->A & 0x80);
}

void lda_absolute(CPU *cpu)
{
    uint16_t addr = memory[cpu->PC] | (memory[cpu->PC + 1] << 8);

    cpu->A = memory[addr];
    cpu->PC += 2;

    set_flag(cpu, FLAG_Z, cpu->A == 0);
    set_flag(cpu, FLAG_N, cpu->A & 0x80);
}

void ldx_immediate(CPU *cpu)
{
    cpu->X = memory[cpu->PC++];

    set_flag(cpu, FLAG_Z, cpu->X == 0);
    set_flag(cpu, FLAG_N, cpu->X & 0x80);
}

void ldx_absolute(CPU *cpu)
{
    uint16_t addr = memory[cpu->PC] | (memory[cpu->PC + 1] << 8);

    cpu->X = memory[addr];
    cpu->PC += 2;

    set_flag(cpu, FLAG_Z, cpu->X == 0);
    set_flag(cpu, FLAG_N, cpu->X & 0x80);
}

void ldy_immediate(CPU *cpu)
{
    cpu->Y = memory[cpu->PC++];

    set_flag(cpu, FLAG_Z, cpu->Y == 0);
    set_flag(cpu, FLAG_N, cpu->Y & 0x80);
}

void ldy_absolute(CPU *cpu)
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

int main()
{
    CPU cpu6502;
    memset(&cpu6502, 0, sizeof(cpu6502)); // Initialize CPU registers 0
    memset(&memory, 0, sizeof(memory));   // Initialize memory 0

    memory[0x1000] = 0x50; // value passed to instruction

    memory[0] = 0xAC; // instruction LDY_ABS opcode

    memory[1] = 0x00; // low byte
    memory[2] = 0x10; // high byte

    memory[3] = 0xA9; // instruction LDA_IMM opcode
    memory[4] = 0x10; // value passed to instruction

    memory[6] = 0xC8; // increment y

    memory[5] = 0xAA; // instruction TAX opcode

    uint8_t opcode;
    int done = 0;
    while (!done)
    {
        opcode = memory[cpu6502.PC++];

        switch (opcode)
        {
        case OP_LDA_IMM:
            lda_immediate(&cpu6502);
            break;
        case OP_LDA_ABS:
            lda_absolute(&cpu6502);
            break;
        case OP_LDX_IMM:
            ldx_immediate(&cpu6502);
            break;
        case OP_LDX_ABS:
            ldx_absolute(&cpu6502);
            break;
        case OP_LDY_IMM:
            ldy_immediate(&cpu6502);
            break;
        case OP_LDY_ABS:
            ldy_absolute(&cpu6502);
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
    printf("PC: %d\n", cpu6502.PC);

    return 0;
}
