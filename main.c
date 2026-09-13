#include <stdio.h>
#include <memory.h>
#include "6502Emu.h"

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
