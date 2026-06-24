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

// Instruction set
void lda_immediate(CPU *cpu)
{
    cpu->A = memory[cpu->PC++];
}

void lda_absolute(CPU *cpu)
{
    uint16_t addr = memory[cpu->PC] | (memory[cpu->PC + 1] << 8);

    cpu->A = memory[addr];
    cpu->PC += 2;
}

void ldx_immediate(CPU *cpu)
{
    cpu->X = memory[cpu->PC++];
}

void ldx_absolute(CPU *cpu)
{
    uint16_t addr = memory[cpu->PC] | (memory[cpu->PC + 1] << 8);

    cpu->X = memory[addr];
    cpu->PC += 2;
}

void ldy_immediate(CPU *cpu)
{
    cpu->Y = memory[cpu->PC++];
}

void ldy_absolute(CPU *cpu)
{
    uint16_t addr = memory[cpu->PC] | (memory[cpu->PC + 1] << 8);

    cpu->Y = memory[addr];
    cpu->PC += 2;
}

int main()
{
    CPU cpu6502;
    memset(&cpu6502, 0, sizeof(cpu6502)); // Initialize CPU registers 0
    memset(&memory, 0, sizeof(memory));   // Initialize memory 0

    memory[0] = 0xAC; // instruction LDY_ABS opcode

    memory[1] = 0x00; // low byte
    memory[2] = 0x10; // high byte

    memory[3] = 0xA9; // instruction LDA_IMM opcode
    memory[4] = 0x10; // value passed to instruction

    memory[0x1000] = 0x50; // value passed to instruction

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
    printf("PC: %d\n", cpu6502.PC);

    return 0;
}
