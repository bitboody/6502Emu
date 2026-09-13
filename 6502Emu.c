#include "6502Emu.h"

uint8_t memory[65536];

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
