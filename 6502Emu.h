#ifndef EMU6502_H
#define EMU6502_H

#include <stdint.h>

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

extern uint8_t memory[65536];

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
void update_zn(CPU *cpu, int8_t value);
void update_c(CPU *cpu, int16_t value);
void update_c_sbc(CPU *cpu, uint8_t old_a, uint8_t borrow, int16_t operand);
void update_v(CPU *cpu, uint8_t a, uint8_t b, uint8_t result);
void update_v_sbc(CPU *cpu, uint8_t a, uint8_t b, uint8_t result);
uint16_t fetch_word(CPU *cpu);

// Instruction set
void clc(CPU *cpu);
void sec(CPU *cpu);
void cli(CPU *cpu);
void sei(CPU *cpu);
void clv(CPU *cpu);
void cld(CPU *cpu);
void sed(CPU *cpu);

void and_imm(CPU *cpu);
void and_zp(CPU *cpu);
void and_zp_x(CPU *cpu);
void and_abs(CPU *cpu);
void and_abs_x(CPU *cpu);
void and_abs_y(CPU *cpu);
void and_indir_x(CPU *cpu);
void and_indir_y(CPU *cpu);

void ora_imm(CPU *cpu);
void ora_zp(CPU *cpu);
void ora_zp_x(CPU *cpu);
void ora_abs(CPU *cpu);
void ora_abs_x(CPU *cpu);
void ora_abs_y(CPU *cpu);
void ora_indir_x(CPU *cpu);
void ora_indir_y(CPU *cpu);

void eor_imm(CPU *cpu);
void eor_zp(CPU *cpu);
void eor_zp_x(CPU *cpu);
void eor_abs(CPU *cpu);
void eor_abs_x(CPU *cpu);
void eor_abs_y(CPU *cpu);
void eor_indir_x(CPU *cpu);
void eor_indir_y(CPU *cpu);

void lda_imm(CPU *cpu);
void lda_abs(CPU *cpu);
void ldx_imm(CPU *cpu);
void ldx_abs(CPU *cpu);
void ldy_imm(CPU *cpu);
void ldy_abs(CPU *cpu);

void tax(CPU *cpu);
void tay(CPU *cpu);
void tya(CPU *cpu);
void txa(CPU *cpu);
void tsx(CPU *cpu);
void txs(CPU *cpu);

void inx(CPU *cpu);
void iny(CPU *cpu);
void dex(CPU *cpu);
void dey(CPU *cpu);

void sta_zp(CPU *cpu);
void sta_zp_x(CPU *cpu);
void sta_abs(CPU *cpu);
void sta_abs_x(CPU *cpu);
void sta_abs_y(CPU *cpu);
void sta_indir_x(CPU *cpu);
void sta_indir_y(CPU *cpu);
void stx_zp(CPU *cpu);
void stx_zp_y(CPU *cpu);
void stx_abs(CPU *cpu);
void sty_zp(CPU *cpu);
void sty_zp_x(CPU *cpu);
void sty_abs(CPU *cpu);

void adc_imm(CPU *cpu);
void adc_abs(CPU *cpu);
void adc_abs_x(CPU *cpu);
void adc_abs_y(CPU *cpu);
void adc_zp(CPU *cpu);
void adc_zp_x(CPU *cpu);
void adc_indir_x(CPU *cpu);
void adc_indir_y(CPU *cpu);

void sbc_imm(CPU *cpu);
void sbc_abs(CPU *cpu);
void sbc_abs_x(CPU *cpu);
void sbc_abs_y(CPU *cpu);
void sbc_zp(CPU *cpu);
void sbc_zp_x(CPU *cpu);
void sbc_indir_x(CPU *cpu);
void sbc_indir_y(CPU *cpu);

void jmp_abs(CPU *cpu);
void jmp_indir(CPU *cpu);

#endif // EMU6502_H
