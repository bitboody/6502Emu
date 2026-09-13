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
#define OP_CLC 0x18
#define OP_SEC 0x38
#define OP_CLI 0x58
#define OP_SEI 0x78
#define OP_CLV 0xB8
#define OP_CLD 0xD8
#define OP_SED 0xF8

#define OP_LDA_IMM 0xA9
#define OP_LDA_ABS 0xAD
#define OP_LDX_IMM 0xA2
#define OP_LDX_ABS 0xAE
#define OP_LDY_IMM 0xA0
#define OP_LDY_ABS 0xAC

#define OP_AND_INDIR_X 0x21
#define OP_AND_ZP 0x25
#define OP_AND_IMM 0x29
#define OP_AND_ZP_X 0x35
#define OP_AND_ABS 0x2D
#define OP_AND_INDIR_Y 0x31
#define OP_AND_ABS_Y 0x39
#define OP_AND_ABS_X 0x3D

#define OP_ORA_INDIR_X 0x01
#define OP_ORA_ZP 0x05
#define OP_ORA_IMM 0x09
#define OP_ORA_ZP_X 0x15
#define OP_ORA_ABS 0x0D
#define OP_ORA_INDIR_Y 0x11
#define OP_ORA_ABS_Y 0x19
#define OP_ORA_ABS_X 0x1D

#define OP_EOR_INDIR_X 0x41
#define OP_EOR_ZP 0x45
#define OP_EOR_IMM 0x49
#define OP_EOR_ZP_X 0x55
#define OP_EOR_ABS 0x4D
#define OP_EOR_INDIR_Y 0x51
#define OP_EOR_ABS_Y 0x59
#define OP_EOR_ABS_X 0x5D

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

#define OP_STA_ZP 0x85
#define OP_STA_ZP_X 0x95
#define OP_STA_ABS 0x8D
#define OP_STA_ABS_X 0x9D
#define OP_STA_ABS_Y 0x99
#define OP_STA_INDIR_X 0x81
#define OP_STA_INDIR_Y 0x91
#define OP_STX_ZP 0x86
#define OP_STX_ZP_Y 0x96
#define OP_STX_ABS 0x8E
#define OP_STY_ZP 0x84
#define OP_STY_ZP_X 0x94
#define OP_STY_ABS 0x8C

#define OP_ADC_IMM 0x69
#define OP_ADC_ABS 0x6D
#define OP_ADC_ABS_X 0x7D
#define OP_ADC_ABS_Y 0x79
#define OP_ADC_ZP 0x65
#define OP_ADC_ZP_X 0x75
#define OP_ADC_INDIR_X 0x61
#define OP_ADC_INDIR_Y 0x71

#define OP_SBC_IMM 0xE9
#define OP_SBC_ABS 0xED
#define OP_SBC_ABS_X 0xFD
#define OP_SBC_ABS_Y 0xF9
#define OP_SBC_ZP 0xE5
#define OP_SBC_ZP_X 0xF5
#define OP_SBC_INDIR_X 0xE1
#define OP_SBC_INDIR_Y 0xF1

#define OP_JMP_ABS 0x4C
#define OP_JMP_INDIR 0x6C

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

void update_c_sbc(CPU *cpu, uint8_t old_a, uint8_t borrow, int16_t operand)
{
    if (old_a >= operand + borrow)
        cpu->P |= FLAG_C;
    else
        cpu->P &= ~FLAG_C;
}

void update_v(CPU *cpu, uint8_t a, uint8_t b, uint8_t result)
{
    if (~(a ^ b) & (a ^ result) & 0x80)
        cpu->P |= FLAG_V;
    else
        cpu->P &= ~FLAG_V;
}

void update_v_sbc(CPU *cpu, uint8_t a, uint8_t b, uint8_t result)
{
    if ((a ^ b) & (a ^ result) & 0x80)
        cpu->P |= FLAG_V;
    else
        cpu->P &= ~FLAG_V;
}

uint16_t fetch_word(CPU *cpu)
{
    uint16_t addr = memory[cpu->PC] | (memory[cpu->PC + 1] << 8);
    cpu->PC += 2;

    return addr;
}

// Instruction set
void clc(CPU *cpu)
{
    cpu->P &= ~FLAG_C;
}

void sec(CPU *cpu)
{
    cpu->P |= FLAG_C;
}

void cli(CPU *cpu)
{
    cpu->P &= ~FLAG_I;
}

void sei(CPU *cpu)
{
    cpu->P |= FLAG_I;
}

void clv(CPU *cpu)
{
    cpu->P &= ~FLAG_V;
}

void cld(CPU *cpu)
{
    cpu->P &= ~FLAG_D;
}

void sed(CPU *cpu)
{
    cpu->P |= FLAG_D;
}

void and_imm(CPU *cpu)
{
    cpu->A &= memory[cpu->PC++];

    update_zn(cpu, cpu->A);
}

void and_zp(CPU *cpu)
{
    uint8_t addr = memory[cpu->PC++];
    cpu->A &= memory[addr];

    update_zn(cpu, cpu->A);
}

void and_zp_x(CPU *cpu)
{
    uint8_t addr = memory[cpu->PC++] + cpu->X;
    cpu->A &= memory[addr];

    update_zn(cpu, cpu->A);
}

void and_abs(CPU *cpu)
{
    uint16_t addr = fetch_word(cpu);
    cpu->A &= memory[addr];

    update_zn(cpu, cpu->A);
}

void and_abs_x(CPU *cpu)
{
    uint16_t addr = fetch_word(cpu) + cpu->X;
    cpu->A &= memory[addr];

    update_zn(cpu, cpu->A);
}

void and_abs_y(CPU *cpu)
{
    uint16_t addr = fetch_word(cpu) + cpu->Y;
    cpu->A &= memory[addr];

    update_zn(cpu, cpu->A);
}

void and_indir_x(CPU *cpu)
{
    uint8_t operand = memory[cpu->PC++] + cpu->X;

    uint8_t low = memory[(uint8_t)operand];
    uint8_t high = memory[(uint8_t)(operand + 1)];
    uint16_t addr = (uint16_t)low | ((uint16_t)high << 8);

    cpu->A &= memory[addr];

    update_zn(cpu, cpu->A);
}

void and_indir_y(CPU *cpu)
{
    uint8_t operand = memory[cpu->PC++];

    uint8_t low = memory[(uint8_t)operand];
    uint8_t high = memory[(uint8_t)(operand + 1)];
    uint16_t addr = ((uint16_t)low | ((uint16_t)high << 8)) + cpu->Y;

    cpu->A &= memory[addr];

    update_zn(cpu, cpu->A);
}

void ora_imm(CPU *cpu)
{
    cpu->A |= memory[cpu->PC++];

    update_zn(cpu, cpu->A);
}

void ora_zp(CPU *cpu)
{
    uint8_t addr = memory[cpu->PC++];
    cpu->A |= memory[addr];

    update_zn(cpu, cpu->A);
}

void ora_zp_x(CPU *cpu)
{
    uint8_t addr = memory[cpu->PC++] + cpu->X;
    cpu->A |= memory[addr];

    update_zn(cpu, cpu->A);
}

void ora_abs(CPU *cpu)
{
    uint16_t addr = fetch_word(cpu);
    cpu->A |= memory[addr];

    update_zn(cpu, cpu->A);
}

void ora_abs_x(CPU *cpu)
{
    uint16_t addr = fetch_word(cpu) + cpu->X;
    cpu->A |= memory[addr];

    update_zn(cpu, cpu->A);
}

void ora_abs_y(CPU *cpu)
{
    uint16_t addr = fetch_word(cpu) + cpu->Y;
    cpu->A |= memory[addr];

    update_zn(cpu, cpu->A);
}

void ora_indir_x(CPU *cpu)
{
    uint8_t operand = memory[cpu->PC++] + cpu->X;

    uint8_t low = memory[(uint8_t)operand];
    uint8_t high = memory[(uint8_t)(operand + 1)];
    uint16_t addr = (uint16_t)low | ((uint16_t)high << 8);

    cpu->A |= memory[addr];

    update_zn(cpu, cpu->A);
}

void ora_indir_y(CPU *cpu)
{
    uint8_t operand = memory[cpu->PC++];

    uint8_t low = memory[(uint8_t)operand];
    uint8_t high = memory[(uint8_t)(operand + 1)];
    uint16_t addr = ((uint16_t)low | ((uint16_t)high << 8)) + cpu->Y;

    cpu->A |= memory[addr];

    update_zn(cpu, cpu->A);
}

void eor_imm(CPU *cpu)
{
    cpu->A ^= memory[cpu->PC++];

    update_zn(cpu, cpu->A);
}

void eor_zp(CPU *cpu)
{
    uint8_t addr = memory[cpu->PC++];
    cpu->A ^= memory[addr];

    update_zn(cpu, cpu->A);
}

void eor_zp_x(CPU *cpu)
{
    uint8_t addr = memory[cpu->PC++] + cpu->X;
    cpu->A ^= memory[addr];

    update_zn(cpu, cpu->A);
}

void eor_abs(CPU *cpu)
{
    uint16_t addr = fetch_word(cpu);
    cpu->A ^= memory[addr];

    update_zn(cpu, cpu->A);
}

void eor_abs_x(CPU *cpu)
{
    uint16_t addr = fetch_word(cpu) + cpu->X;
    cpu->A ^= memory[addr];

    update_zn(cpu, cpu->A);
}

void eor_abs_y(CPU *cpu)
{
    uint16_t addr = fetch_word(cpu) + cpu->Y;
    cpu->A ^= memory[addr];

    update_zn(cpu, cpu->A);
}

void eor_indir_x(CPU *cpu)
{
    uint8_t operand = memory[cpu->PC++] + cpu->X;

    uint8_t low = memory[(uint8_t)operand];
    uint8_t high = memory[(uint8_t)(operand + 1)];
    uint16_t addr = (uint16_t)low | ((uint16_t)high << 8);

    cpu->A ^= memory[addr];

    update_zn(cpu, cpu->A);
}

void eor_indir_y(CPU *cpu)
{
    uint8_t operand = memory[cpu->PC++];

    uint8_t low = memory[(uint8_t)operand];
    uint8_t high = memory[(uint8_t)(operand + 1)];
    uint16_t addr = ((uint16_t)low | ((uint16_t)high << 8)) + cpu->Y;

    cpu->A ^= memory[addr];

    update_zn(cpu, cpu->A);
}

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

void sta_zp(CPU *cpu)
{
    uint8_t addr = memory[cpu->PC++];
    memory[addr] = cpu->A;
}

void sta_zp_x(CPU *cpu)
{
    uint8_t addr = memory[cpu->PC++] + cpu->X;
    memory[addr] = cpu->A;
}

void sta_abs(CPU *cpu)
{
    uint16_t addr = fetch_word(cpu);
    memory[addr] = cpu->A;
}

void sta_abs_x(CPU *cpu)
{
    uint16_t addr = fetch_word(cpu) + cpu->X;
    memory[addr] = cpu->A;
}

void sta_abs_y(CPU *cpu)
{
    uint16_t addr = fetch_word(cpu) + cpu->Y;
    memory[addr] = cpu->A;
}

void sta_indir_x(CPU *cpu)
{
    uint8_t operand = memory[cpu->PC++] + cpu->X;

    uint8_t low = memory[(uint8_t)operand];
    uint8_t high = memory[(uint8_t)(operand + 1)];
    uint16_t addr = (uint16_t)low | ((uint16_t)high << 8);

    memory[addr] = cpu->A;
}

void sta_indir_y(CPU *cpu)
{
    uint8_t operand = memory[cpu->PC++];

    uint8_t low = memory[(uint8_t)operand];
    uint8_t high = memory[(uint8_t)(operand + 1)];
    uint16_t addr = ((uint16_t)low | ((uint16_t)high << 8)) + cpu->Y;

    memory[addr] = cpu->A;
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

void sty_zp(CPU *cpu)
{
    uint8_t addr = memory[cpu->PC++];
    memory[addr] = cpu->Y;
}

void sty_zp_x(CPU *cpu)
{
    uint8_t addr = memory[cpu->PC++] + cpu->X;
    memory[addr] = cpu->Y;
}

void sty_abs(CPU *cpu)
{
    uint16_t addr = fetch_word(cpu);
    memory[addr] = cpu->Y;
}

void adc_imm(CPU *cpu)
{
    uint8_t old_a = cpu->A;

    uint8_t value = memory[cpu->PC++];
    uint8_t carry = (cpu->P & FLAG_C) ? 1 : 0;

    uint16_t result = old_a + value + carry;

    update_c(cpu, result);

    cpu->A = (uint8_t)result;

    update_v(cpu, old_a, value, cpu->A);
    update_zn(cpu, cpu->A);
}

void adc_abs(CPU *cpu)
{
    uint8_t old_a = cpu->A;

    uint16_t addr = fetch_word(cpu);
    uint8_t carry = (cpu->P & FLAG_C) ? 1 : 0;
    uint8_t value = memory[addr];

    uint16_t result = old_a + value + carry;

    update_c(cpu, result);

    cpu->A = (uint8_t)result;

    update_v(cpu, old_a, value, cpu->A);
    update_zn(cpu, cpu->A);
}

void adc_abs_x(CPU *cpu)
{
    uint8_t old_a = cpu->A;

    uint16_t addr = fetch_word(cpu) + cpu->X;
    uint8_t carry = (cpu->P & FLAG_C) ? 1 : 0;
    uint8_t value = memory[addr];

    uint16_t result = old_a + value + carry;

    update_c(cpu, result);

    cpu->A = (uint8_t)result;

    update_v(cpu, old_a, value, cpu->A);
    update_zn(cpu, cpu->A);
}

void adc_abs_y(CPU *cpu)
{
    uint8_t old_a = cpu->A;

    uint16_t addr = fetch_word(cpu) + cpu->Y;
    uint8_t carry = (cpu->P & FLAG_C) ? 1 : 0;
    uint8_t value = memory[addr];

    uint16_t result = old_a + value + carry;

    update_c(cpu, result);

    cpu->A = (uint8_t)result;

    update_v(cpu, old_a, value, cpu->A);
    update_zn(cpu, cpu->A);
}

void adc_zp(CPU *cpu)
{
    uint8_t old_a = cpu->A;

    uint8_t addr = memory[cpu->PC++];
    uint8_t carry = (cpu->P & FLAG_C) ? 1 : 0;
    uint8_t value = memory[addr];

    uint16_t result = old_a + value + carry;

    update_c(cpu, result);

    cpu->A = (uint8_t)result;

    update_v(cpu, old_a, value, cpu->A);
    update_zn(cpu, cpu->A);
}

void adc_zp_x(CPU *cpu)
{
    uint8_t old_a = cpu->A;

    uint8_t addr = memory[cpu->PC++] + cpu->X;
    uint8_t carry = (cpu->P & FLAG_C) ? 1 : 0;
    uint8_t value = memory[addr];
    uint16_t result = old_a + value + carry;

    update_c(cpu, result);

    cpu->A = (uint8_t)result;

    update_v(cpu, old_a, value, cpu->A);
    update_zn(cpu, cpu->A);
}

void adc_indir_x(CPU *cpu)
{
    uint8_t old_a = cpu->A;

    uint8_t operand = memory[cpu->PC++] + cpu->X;

    uint8_t low = memory[(uint8_t)operand];
    uint8_t high = memory[(uint8_t)(operand + 1)];
    uint16_t addr = (uint16_t)low | ((uint16_t)high << 8);
    uint8_t value = memory[addr];

    uint8_t carry = (cpu->P & FLAG_C) ? 1 : 0;

    uint16_t result = old_a + value + carry;

    update_c(cpu, result);

    cpu->A = (uint8_t)result;

    update_v(cpu, old_a, value, cpu->A);
    update_zn(cpu, cpu->A);
}

void adc_indir_y(CPU *cpu)
{
    uint8_t old_a = cpu->A;

    uint8_t operand = memory[cpu->PC++];

    uint8_t low = memory[(uint8_t)operand];
    uint8_t high = memory[(uint8_t)(operand + 1)];
    uint16_t addr = ((uint16_t)low | ((uint16_t)high << 8)) + cpu->Y;
    uint8_t value = memory[addr];

    uint8_t carry = (cpu->P & FLAG_C) ? 1 : 0;

    uint16_t result = old_a + value + carry;

    update_c(cpu, result);

    cpu->A = (uint8_t)result;

    update_v(cpu, old_a, value, cpu->A);
    update_zn(cpu, cpu->A);
}

void sbc_imm(CPU *cpu)
{
    uint8_t old_a = cpu->A;

    uint8_t value = memory[cpu->PC++];
    uint8_t carry = (cpu->P & FLAG_C) ? 1 : 0;
    uint8_t borrow = 1 - carry;

    uint16_t result = old_a - value - borrow;

    update_c_sbc(cpu, old_a, borrow, value);

    cpu->A = (uint8_t)result;

    update_v_sbc(cpu, old_a, value, cpu->A);
    update_zn(cpu, cpu->A);
}

void sbc_abs(CPU *cpu)
{
    uint8_t old_a = cpu->A;

    uint16_t addr = fetch_word(cpu);
    uint8_t carry = (cpu->P & FLAG_C) ? 1 : 0;
    uint8_t value = memory[addr];
    uint8_t borrow = 1 - carry;

    uint16_t result = old_a - value - borrow;

    update_c_sbc(cpu, old_a, borrow, value);

    cpu->A = (uint8_t)result;

    update_v_sbc(cpu, old_a, value, cpu->A);
    update_zn(cpu, cpu->A);
}

void sbc_abs_x(CPU *cpu)
{
    uint8_t old_a = cpu->A;

    uint16_t addr = fetch_word(cpu) + cpu->X;
    uint8_t carry = (cpu->P & FLAG_C) ? 1 : 0;
    uint8_t value = memory[addr];
    uint8_t borrow = 1 - carry;

    uint16_t result = old_a - value - borrow;

    update_c_sbc(cpu, old_a, borrow, value);

    cpu->A = (uint8_t)result;

    update_v_sbc(cpu, old_a, value, cpu->A);
    update_zn(cpu, cpu->A);
}

void sbc_abs_y(CPU *cpu)
{
    uint8_t old_a = cpu->A;

    uint16_t addr = fetch_word(cpu) + cpu->Y;
    uint8_t carry = (cpu->P & FLAG_C) ? 1 : 0;
    uint8_t value = memory[addr];
    uint8_t borrow = 1 - carry;

    uint16_t result = old_a - value - borrow;

    update_c_sbc(cpu, old_a, borrow, value);

    cpu->A = (uint8_t)result;

    update_v_sbc(cpu, old_a, value, cpu->A);
    update_zn(cpu, cpu->A);
}

void sbc_zp(CPU *cpu)
{
    uint8_t old_a = cpu->A;

    uint8_t addr = memory[cpu->PC++];
    uint8_t carry = (cpu->P & FLAG_C) ? 1 : 0;
    uint8_t value = memory[addr];
    uint8_t borrow = 1 - carry;

    uint16_t result = old_a - value - borrow;

    update_c_sbc(cpu, old_a, borrow, value);

    cpu->A = (uint8_t)result;

    update_v_sbc(cpu, old_a, value, cpu->A);
    update_zn(cpu, cpu->A);
}

void sbc_zp_x(CPU *cpu)
{
    uint8_t old_a = cpu->A;

    uint8_t addr = memory[cpu->PC++] + cpu->X;
    uint8_t carry = (cpu->P & FLAG_C) ? 1 : 0;
    uint8_t value = memory[addr];
    uint8_t borrow = 1 - carry;

    uint16_t result = old_a - value - borrow;

    update_c_sbc(cpu, old_a, borrow, value);

    cpu->A = (uint8_t)result;

    update_v_sbc(cpu, old_a, value, cpu->A);
    update_zn(cpu, cpu->A);
}

void sbc_indir_x(CPU *cpu)
{
    uint8_t old_a = cpu->A;

    uint8_t operand = memory[cpu->PC++] + cpu->X;

    uint8_t low = memory[(uint8_t)operand];
    uint8_t high = memory[(uint8_t)(operand + 1)];
    uint16_t addr = (uint16_t)low | ((uint16_t)high << 8);
    uint8_t value = memory[addr];

    uint8_t carry = (cpu->P & FLAG_C) ? 1 : 0;
    uint8_t borrow = 1 - carry;

    uint16_t result = old_a - value - borrow;

    update_c_sbc(cpu, old_a, borrow, value);

    cpu->A = (uint8_t)result;

    update_v_sbc(cpu, old_a, value, cpu->A);
    update_zn(cpu, cpu->A);
}

void sbc_indir_y(CPU *cpu)
{
    uint8_t old_a = cpu->A;

    uint8_t operand = memory[cpu->PC++];

    uint8_t low = memory[(uint8_t)operand];
    uint8_t high = memory[(uint8_t)(operand + 1)];

    uint16_t addr = ((uint16_t)low | ((uint16_t)high << 8)) + cpu->Y;
    uint8_t value = memory[addr];

    uint8_t carry = (cpu->P & FLAG_C) ? 1 : 0;
    uint8_t borrow = 1 - carry;

    uint16_t result = old_a - value - borrow;

    update_c_sbc(cpu, old_a, borrow, value);

    cpu->A = (uint8_t)result;

    update_v_sbc(cpu, old_a, value, cpu->A);
    update_zn(cpu, cpu->A);
}

void jmp_abs(CPU *cpu)
{
    cpu->PC = fetch_word(cpu);
}

void jmp_indir(CPU *cpu)
{
    uint16_t ptr = fetch_word(cpu);

    uint8_t low = memory[ptr];
    uint16_t high_addr = (ptr & 0xFF00) | ((ptr + 1) & 0x00FF);
    uint8_t high = memory[high_addr];

    uint16_t addr = ((uint16_t)low | ((uint16_t)high << 8));

    cpu->PC = addr;
}

int main()
{
    CPU cpu6502;
    memset(&cpu6502, 0, sizeof(cpu6502)); // Initialize CPU registers 0
    memset(&memory, 0, sizeof(memory));   // Initialize memory 0

    memory[0x0000] = OP_LDA_IMM;
    memory[0x0001] = 0xFF;
    memory[0x0002] = OP_AND_IMM;
    memory[0x0003] = 0xF0;
    memory[0x0004] = OP_AND_ZP;
    memory[0x0005] = 0x40;
    memory[0x0006] = OP_LDX_IMM;
    memory[0x0007] = 0x01;
    memory[0x0008] = OP_AND_ZP_X;
    memory[0x0009] = 0x40;
    memory[0x000A] = OP_AND_ABS;
    memory[0x000B] = 0x00;
    memory[0x000C] = 0x20;
    memory[0x000D] = OP_AND_ABS_X;
    memory[0x000E] = 0x00;
    memory[0x000F] = 0x20;
    memory[0x0010] = OP_LDY_IMM;
    memory[0x0011] = 0x01;
    memory[0x0012] = OP_AND_ABS_Y;
    memory[0x0013] = 0x00;
    memory[0x0014] = 0x20;
    memory[0x0015] = OP_AND_INDIR_X;
    memory[0x0016] = 0x1F;
    memory[0x0017] = OP_AND_INDIR_Y;
    memory[0x0018] = 0x22;
    memory[0x0019] = OP_BRK;

    memory[0x0040] = 0xF3;
    memory[0x0041] = 0xCF;
    memory[0x2000] = 0xBF;
    memory[0x2001] = 0xF7;
    memory[0x0020] = 0x00;
    memory[0x0021] = 0x30;
    memory[0x0022] = 0x00;
    memory[0x0023] = 0x30;
    memory[0x3000] = 0xDF;
    memory[0x3001] = 0x80;

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
        case OP_CLC:
            clc(&cpu6502);
            break;
        case OP_SEC:
            sec(&cpu6502);
            break;
        case OP_CLI:
            cli(&cpu6502);
            break;
        case OP_SEI:
            sei(&cpu6502);
            break;
        case OP_CLV:
            clv(&cpu6502);
            break;
        case OP_CLD:
            cld(&cpu6502);
            break;
        case OP_SED:
            sed(&cpu6502);
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

        case OP_AND_IMM:
            and_imm(&cpu6502);
            break;
        case OP_AND_ZP:
            and_zp(&cpu6502);
            break;
        case OP_AND_ZP_X:
            and_zp_x(&cpu6502);
            break;
        case OP_AND_ABS:
            and_abs(&cpu6502);
            break;
        case OP_AND_ABS_X:
            and_abs_x(&cpu6502);
            break;
        case OP_AND_ABS_Y:
            and_abs_y(&cpu6502);
            break;
        case OP_AND_INDIR_X:
            and_indir_x(&cpu6502);
            break;
        case OP_AND_INDIR_Y:
            and_indir_y(&cpu6502);
            break;

        case OP_ORA_IMM:
            ora_imm(&cpu6502);
            break;
        case OP_ORA_ZP:
            ora_zp(&cpu6502);
            break;
        case OP_ORA_ZP_X:
            ora_zp_x(&cpu6502);
            break;
        case OP_ORA_ABS:
            ora_abs(&cpu6502);
            break;
        case OP_ORA_ABS_X:
            ora_abs_x(&cpu6502);
            break;
        case OP_ORA_ABS_Y:
            ora_abs_y(&cpu6502);
            break;
        case OP_ORA_INDIR_X:
            ora_indir_x(&cpu6502);
            break;
        case OP_ORA_INDIR_Y:
            ora_indir_y(&cpu6502);
            break;

        case OP_EOR_IMM:
            eor_imm(&cpu6502);
            break;
        case OP_EOR_ZP:
            eor_zp(&cpu6502);
            break;
        case OP_EOR_ZP_X:
            eor_zp_x(&cpu6502);
            break;
        case OP_EOR_ABS:
            eor_abs(&cpu6502);
            break;
        case OP_EOR_ABS_X:
            eor_abs_x(&cpu6502);
            break;
        case OP_EOR_ABS_Y:
            eor_abs_y(&cpu6502);
            break;
        case OP_EOR_INDIR_X:
            eor_indir_x(&cpu6502);
            break;
        case OP_EOR_INDIR_Y:
            eor_indir_y(&cpu6502);
            break;

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
        case OP_STA_ZP:
            sta_zp(&cpu6502);
            break;
        case OP_STA_ZP_X:
            sta_zp_x(&cpu6502);
            break;
        case OP_STA_ABS:
            sta_abs(&cpu6502);
            break;
        case OP_STA_ABS_X:
            sta_abs_x(&cpu6502);
            break;
        case OP_STA_ABS_Y:
            sta_abs_y(&cpu6502);
            break;
        case OP_STA_INDIR_X:
            sta_indir_x(&cpu6502);
            break;
        case OP_STA_INDIR_Y:
            sta_indir_y(&cpu6502);
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
        case OP_STY_ZP:
            sty_zp(&cpu6502);
            break;
        case OP_STY_ZP_X:
            sty_zp_x(&cpu6502);
            break;
        case OP_STY_ABS:
            sty_abs(&cpu6502);
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

            /* ========= SBC ========= */

        case OP_SBC_IMM:
            sbc_imm(&cpu6502);
            break;
        case OP_SBC_ZP:
            sbc_zp(&cpu6502);
            break;
        case OP_SBC_ZP_X:
            sbc_zp_x(&cpu6502);
            break;
        case OP_SBC_ABS:
            sbc_abs(&cpu6502);
            break;
        case OP_SBC_ABS_X:
            sbc_abs_x(&cpu6502);
            break;
        case OP_SBC_ABS_Y:
            sbc_abs_y(&cpu6502);
            break;
        case OP_SBC_INDIR_X:
            sbc_indir_x(&cpu6502);
            break;
        case OP_SBC_INDIR_Y:
            sbc_indir_y(&cpu6502);
            break;

            /* ========= JMP ========= */

        case OP_JMP_ABS:
            jmp_abs(&cpu6502);
            break;
        case OP_JMP_INDIR:
            jmp_indir(&cpu6502);
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

    printf("PC: 0x%02X (%d decimal)\n", cpu6502.PC, cpu6502.PC);

    return 0;
}
