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

#define OP_ADC_IMM 0x69
#define OP_ADC_ABS 0x6D
#define OP_ADC_ABS_X 0x7D
#define OP_ADC_ABS_Y 0x79
#define OP_ADC_ZP 0x65
#define OP_ADC_ZP_X 0x75
#define OP_ADC_INDIR_X 0x61
#define OP_ADC_INDIR_Y 0x71

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
void update_zn(CPU *cpu, int8_t value)
{
    if (value == 0)
        cpu->P |= FLAG_Z;
    else
        cpu->P &= ~FLAG_Z;

    if (value & FLAG_N)
        cpu->P |= FLAG_N;
    else
        cpu->P &= ~FLAG_N;
}

void update_c(CPU *cpu, int16_t value)
{
    if (value > 0xFF)
        cpu->P |= FLAG_C;
    else
        cpu->P &= ~FLAG_C;
}

uint16_t fetch_word(CPU *cpu)
{
    uint16_t addr = memory[cpu->PC] | (memory[cpu->PC + 1] << 8);
    cpu->PC += 2;

    return addr;
}

// Instruction set
void lda_imm(CPU *cpu)
{
    cpu->A = memory[cpu->PC++];

    update_zn(cpu, cpu->A);
}

void lda_abs(CPU *cpu)
{
    uint16_t addr = fetch_word(cpu);
    cpu->A = memory[addr];

    update_zn(cpu, cpu->A);
}

void ldx_imm(CPU *cpu)
{
    cpu->X = memory[cpu->PC++];

    update_zn(cpu, cpu->X);
}

void ldx_abs(CPU *cpu)
{
    uint16_t addr = fetch_word(cpu);
    cpu->X = memory[addr];

    update_zn(cpu, cpu->X);
}

void ldy_imm(CPU *cpu)
{
    cpu->Y = memory[cpu->PC++];

    update_zn(cpu, cpu->Y);
}

void ldy_abs(CPU *cpu)
{
    uint16_t addr = fetch_word(cpu);
    cpu->Y = memory[addr];

    update_zn(cpu, cpu->Y);
}

void tax(CPU *cpu)
{
    cpu->X = cpu->A;

    update_zn(cpu, cpu->X);
}

void tay(CPU *cpu)
{
    cpu->Y = cpu->A;

    update_zn(cpu, cpu->Y);
}

void tya(CPU *cpu)
{
    cpu->A = cpu->Y;

    update_zn(cpu, cpu->A);
}

void txa(CPU *cpu)
{
    cpu->A = cpu->X;

    update_zn(cpu, cpu->A);
}

void tsx(CPU *cpu)
{
    cpu->X = cpu->SP;

    update_zn(cpu, cpu->X);
}

void txs(CPU *cpu) // TXS Does not affect any flags
{
    cpu->SP = cpu->X;
}

void inx(CPU *cpu)
{
    cpu->X++;

    update_zn(cpu, cpu->X);
}

void iny(CPU *cpu)
{
    cpu->Y++;

    update_zn(cpu, cpu->Y);
}

void dex(CPU *cpu)
{
    cpu->X--;

    update_zn(cpu, cpu->X);
}

void dey(CPU *cpu)
{
    cpu->Y--;

    update_zn(cpu, cpu->Y);
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
    uint16_t addr = fetch_word(cpu);
    memory[addr] = cpu->X;
}

void adc_imm(CPU *cpu)
{
    uint8_t value = memory[cpu->PC++];
    uint8_t carry = (cpu->P & FLAG_C) ? 1 : 0;
    uint16_t result = cpu->A + value + carry;

    update_c(cpu, result);

    cpu->A = (uint8_t)result;

    update_zn(cpu, cpu->A);
}

void adc_abs(CPU *cpu)
{
    uint16_t addr = fetch_word(cpu);
    uint8_t carry = (cpu->P & FLAG_C) ? 1 : 0;
    uint16_t result = cpu->A + memory[addr] + carry;

    update_c(cpu, result);

    cpu->A = (uint8_t)result;

    update_zn(cpu, cpu->A);
}

void adc_abs_x(CPU *cpu)
{
    uint16_t addr = fetch_word(cpu) + cpu->X;
    uint8_t carry = (cpu->P & FLAG_C) ? 1 : 0;
    uint16_t result = cpu->A + memory[addr] + carry;

    update_c(cpu, result);

    cpu->A = (uint8_t)result;

    update_zn(cpu, cpu->A);
}

void adc_abs_y(CPU *cpu)
{
    uint16_t addr = fetch_word(cpu) + cpu->Y;
    uint8_t carry = (cpu->P & FLAG_C) ? 1 : 0;
    uint16_t result = cpu->A + memory[addr] + carry;

    update_c(cpu, result);

    cpu->A = (uint8_t)result;

    update_zn(cpu, cpu->A);
}

void adc_zp(CPU *cpu)
{
    uint8_t addr = memory[cpu->PC++];
    uint8_t carry = (cpu->P & FLAG_C) ? 1 : 0;
    uint16_t result = cpu->A + memory[addr] + carry;

    update_c(cpu, result);

    cpu->A = (uint8_t)result;

    update_zn(cpu, cpu->A);
}

void adc_zp_x(CPU *cpu)
{
    uint8_t addr = memory[cpu->PC++] + cpu->X;
    uint8_t carry = (cpu->P & FLAG_C) ? 1 : 0;
    uint16_t result = cpu->A + memory[addr] + carry;

    update_c(cpu, result);

    cpu->A = (uint8_t)result;

    update_zn(cpu, cpu->A);
}

void adc_indir_x(CPU *cpu)
{
    uint8_t operand = memory[cpu->PC++] + cpu->X;
    uint8_t low = memory[operand];
    uint8_t high = memory[(uint8_t)(operand + 1)];

    uint8_t carry = (cpu->P & FLAG_C) ? 1 : 0;

    uint16_t addr = low | (high << 8);
    uint16_t result = cpu->A + memory[addr] + carry;

    update_c(cpu, result);

    cpu->A = (uint8_t)result;

    update_zn(cpu, cpu->A);
}

void adc_indir_y(CPU *cpu)
{
    uint8_t operand = memory[cpu->PC++];
    uint8_t low = memory[operand];
    uint8_t high = memory[(uint8_t)(operand + 1)];

    uint8_t carry = (cpu->P & FLAG_C) ? 1 : 0;

    uint16_t addr = low | ((high << 8) + cpu->Y);
    uint16_t result = cpu->A + memory[addr] + carry;

    update_c(cpu, result);

    cpu->A = (uint8_t)result;

    update_zn(cpu, cpu->A);
}

int main()
{
    CPU cpu6502;
    memset(&cpu6502, 0, sizeof(cpu6502)); // Initialize CPU registers 0
    memset(&memory, 0, sizeof(memory));   // Initialize memory 0

    // Tests
    memory[0] = OP_LDA_IMM;
    memory[1] = 250;

    memory[2] = OP_ADC_IMM;
    memory[3] = 20;

    memory[4] = OP_BRK;

    uint8_t opcode;
    int done = 0;
    while (!done)
    {
        opcode = memory[cpu6502.PC++];

        switch (opcode)
        {
        case OP_BRK:
            done = 1;
            break;

            /* ========= LOAD ========= */

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

            /* ======== TRANSFER ======== */

        case OP_TAX:
            tax(&cpu6502);
            break;
        case OP_TAY:
            tay(&cpu6502);
            break;
        case OP_TXA:
            txa(&cpu6502);
            break;
        case OP_TYA:
            tya(&cpu6502);
            break;
        case OP_TSX:
            tsx(&cpu6502);
            break;
        case OP_TXS:
            txs(&cpu6502);
            break;

            /* ===== INCREMENT / DECREMENT ===== */

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

            /* ========= STORE ========= */

        case OP_STX_ZP:
            stx_zp(&cpu6502);
            break;
        case OP_STX_ZP_Y:
            stx_zp_y(&cpu6502);
            break;
        case OP_STX_ABS:
            stx_abs(&cpu6502);
            break;

            /* ========= ADC ========= */

        case OP_ADC_IMM:
            adc_imm(&cpu6502);
            break;
        case OP_ADC_ZP:
            adc_zp(&cpu6502);
            break;
        case OP_ADC_ZP_X:
            adc_zp_x(&cpu6502);
            break;
        case OP_ADC_ABS:
            adc_abs(&cpu6502);
            break;
        case OP_ADC_ABS_X:
            adc_abs_x(&cpu6502);
            break;
        case OP_ADC_ABS_Y:
            adc_abs_y(&cpu6502);
            break;
        case OP_ADC_INDIR_X:
            adc_indir_x(&cpu6502);
            break;
        case OP_ADC_INDIR_Y:
            adc_indir_y(&cpu6502);
            break;

        default:
            printf("Unknown opcode: 0x%02X\n", opcode);
            done = 1;
            break;
        }
    }

    printf("Register A: 0x%02X (%d decimal)\n", cpu6502.A, cpu6502.A);
    printf("Register X: 0x%02X (%d decimal)\n", cpu6502.X, cpu6502.X);
    printf("Register Y: 0x%02X (%d decimal)\n", cpu6502.Y, cpu6502.Y);
    printf("Register SP: 0x%02X (%d decimal)\n", cpu6502.SP, cpu6502.SP);
    printf(
        "P: N=%d V=%d U=%d B=%d D=%d I=%d Z=%d C=%d (0x%02X)\n",
        (cpu6502.P & FLAG_N) != 0,
        (cpu6502.P & FLAG_V) != 0,
        (cpu6502.P & FLAG_U) != 0,
        (cpu6502.P & FLAG_B) != 0,
        (cpu6502.P & FLAG_D) != 0,
        (cpu6502.P & FLAG_I) != 0,
        (cpu6502.P & FLAG_Z) != 0,
        (cpu6502.P & FLAG_C) != 0,
        cpu6502.P);

    printf("PC: %d\n", cpu6502.PC);

    return 0;
}
