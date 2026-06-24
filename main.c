#include <stdint.h>
#include <stdio.h>
#include <memory.h>

#define FLAG_C 0x01
#define FLAG_Z 0x02
#define FLAG_I 0x04
#define FLAG_D 0x08
#define FLAG_B 0x10
#define FLAG_U 0x20
#define FLAG_V 0x40
#define FLAG_N 0x80

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
    cpu->PC++;
}

void ldx_immediate(CPU *cpu)
{
    cpu->X = memory[cpu->PC++];
}

void ldx_absolute(CPU *cpu)
{
    uint16_t addr = memory[cpu->PC] | (memory[cpu->PC + 1] << 8);

    cpu->X = memory[addr];
    cpu->PC++;
}

void ldy_immediate(CPU *cpu)
{
    cpu->Y = memory[cpu->PC++];
}

void ldy_absolute(CPU *cpu)
{
    uint16_t addr = memory[cpu->PC] | (memory[cpu->PC + 1] << 8);

    cpu->Y = memory[addr];
    cpu->PC++;
}

int main()
{
    CPU cpu6502;
    memset(&cpu6502, 0, sizeof(cpu6502)); // Initialize CPU registers 0
    memset(&memory, 0, sizeof(memory));   // Initialize memory 0

    memory[0] = 0xAD; // instruction LDA opcode

    memory[1] = 0x00; // low byte
    memory[2] = 0x10; // high byte

    memory[3] = 0xA9; // instruction LDA opcode
    memory[4] = 0x10; // value passed to instruction

    memory[0x1000] = 0x50; // value passed to instruction

    uint8_t opcode;
    int done = 0;
    while (!done)
    {
        opcode = memory[cpu6502.PC++];

        switch (opcode)
        {
        case 0xA9:
            lda_immediate(&cpu6502);
            break;
        case 0xAD:
            lda_absolute(&cpu6502);
            break;
        case 0xA2:
            ldx_immediate(&cpu6502);
            break;
        case 0xAE:
            ldx_absolute(&cpu6502);
            break;
        case 0xA0:
            ldy_immediate(&cpu6502);
            break;
        case 0xAC:
            ldy_absolute(&cpu6502);
            break;
        case 0x00:
            done = 1;
        default:
            printf("Unknown opcode: 0x%02X\n", opcode);
            done = 1;
            break;
        }
    }

    printf("Register A: 0x%02X\n", cpu6502.A);
    printf("PC: %d\n", cpu6502.PC);

    return 0;
}
