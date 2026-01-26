#include "common.h"
#include <stdlib.h>
#include <string.h>

void strchara(char s[], char n) {
    int len = strlen(s);
    s[len] = n;
    s[len+1] = '\0';
}

char regnames[33][4] = {
    "r0", "r1", "r2", "r3", "r4",
    "r5", "r6", "r7", "r8", "r9",
    "r10", "r11", "r12", "r13", "r14",
    "r15", "r16", "r17", "r18", "r19",
    "r20", "r21", "r22", "r23", "r24",
    "r25", "r26", "r27", "r28", "r29",
    "r30", "r31", "rXX"
};
char tempbuf[32];
char jmpnames[16][4] = {
    "mp", "be", "l", "le", "s",
    "z", "o", "c", "n", "nbe",
    "nl", "nle", "ns", "nz", "no",
    "nc",
};

void CORE_reverse(char s[]) {
    int c, i, j;
    for (i = 0, j = strlen(s)-1; i < j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}
char* CORE_itoa(VM_word n) {
    VM_word i;
    i = 0;
    do {
        tempbuf[i++] = n % 10 + '0';
    } while ((n /= 10) > 0);

    tempbuf[i] = '\0';
    CORE_reverse(tempbuf);
    return tempbuf;
}

char* VM_disasminstruction(VM_word instruction) {
    char* buf = (char*)malloc(sizeof(char)*128);
    buf[0] = 0x00;



    uint8_t moi = instruction >> 31; // msb operation index
    uint8_t soii = (instruction >> 30) & 0x1; // second operand is an immediate value
    uint8_t destreg = (instruction >> 25) & 0b11111; // dest register index
    uint8_t psrcreg = (instruction >> 20) & 0b11111; // pr. src register index
    uint8_t loi = (instruction >> 16) & 0b1111; // lsb operation index
    uint16_t ssrcreg = instruction & 0b1111111111111111; // sec. src register index (immediate value)

    patchword((VM_word*)&ssrcreg);
    if (soii) {
        CORE_itoa(ssrcreg);
    } else {
        if (ssrcreg > 31) { // invalid instruction, make dw
            strcat(buf, "dw ");
            CORE_itoa(instruction);
            strcat(buf, tempbuf);
            return buf;
        }
    }

    // jmp conditions
    uint8_t sync = !(psrcreg >> 4);
    uint8_t condindex = psrcreg & 0b1111;

    switch (loi) {
        case 4: // sub
            strcat(buf, "sub");
            if (!moi) {
                strchara(buf, 's');
            }
            strchara(buf, ' ');
            strcat(buf, regnames[destreg]);
            strcat(buf, ", ");
            strcat(buf, soii ? tempbuf : regnames[ssrcreg]);
            strcat(buf, ", ");
            strcat(buf, regnames[psrcreg]);
            break;
        case 5: // sbb
            strcat(buf, "sbb");
            if (!moi) {
                strchara(buf, 's');
            }
            strchara(buf, ' ');
            strcat(buf, regnames[destreg]);
            strcat(buf, ", ");
            strcat(buf, soii ? tempbuf : regnames[ssrcreg]);
            strcat(buf, ", ");
            strcat(buf, regnames[psrcreg]);
            break;
        case 6: // add
            strcat(buf, "add");
            if (!moi) {
                strchara(buf, 's');
            }
            strchara(buf, ' ');
            strcat(buf, regnames[destreg]);
            strcat(buf, ", ");
            strcat(buf, regnames[psrcreg]);
            strcat(buf, ", ");
            strcat(buf, soii ? tempbuf : regnames[ssrcreg]);
            break;
        case 7: // adc
            strcat(buf, "adc");
            if (!moi) {
                strchara(buf, 's');
            }
            strchara(buf, ' ');
            strcat(buf, regnames[destreg]);
            strcat(buf, ", ");
            strcat(buf, regnames[psrcreg]);
            strcat(buf, ", ");
            strcat(buf, soii ? tempbuf : regnames[ssrcreg]);
            break;
        case 8: // xor
            strcat(buf, "xor");
            if (!moi) {
                strchara(buf, 's');
            }
            strchara(buf, ' ');
            strcat(buf, regnames[destreg]);
            strcat(buf, ", ");
            strcat(buf, regnames[psrcreg]);
            strcat(buf, ", ");
            strcat(buf, soii ? tempbuf : regnames[ssrcreg]);
            break;
        case 9: // or
            strcat(buf, "or");
            if (!moi) {
                strchara(buf, 's');
            }
            strchara(buf, ' ');
            strcat(buf, regnames[destreg]);
            strcat(buf, ", ");
            strcat(buf, regnames[psrcreg]);
            strcat(buf, ", ");
            strcat(buf, soii ? tempbuf : regnames[ssrcreg]);
            break;
        case 11: // bitshift
            if ((ssrcreg >> 15) == 0) {
                strcat(buf, "shl");
                if (!moi) {
                    strchara(buf, 's');
                }
                strchara(buf, ' ');
                strcat(buf, regnames[destreg]);
                strcat(buf, ", ");
                strcat(buf, regnames[psrcreg]);
                strcat(buf, ", ");
                strcat(buf, soii ? tempbuf : regnames[ssrcreg]);
            } else {
                strcat(buf, "shr");
                if (!moi) {
                    strchara(buf, 's');
                }
                strchara(buf, ' ');
                strcat(buf, regnames[destreg]);
                strcat(buf, ", ");
                strcat(buf, regnames[psrcreg]);
                strcat(buf, ", ");
                strcat(buf, soii ? tempbuf : regnames[ssrcreg]);
                }
            break;
        case 12: // and
            strcat(buf, "and");
            if (!moi) {
                strchara(buf, 's');
            }
            strchara(buf, ' ');
            strcat(buf, regnames[destreg]);
            strcat(buf, ", ");
            strcat(buf, regnames[psrcreg]);
            strcat(buf, ", ");
            strcat(buf, soii ? tempbuf : regnames[ssrcreg]);
            break;
        case 13: // hlt
            strcat(buf, "hlt");
            break;
        case 14: // mul or muls
            // check if core is multiply capable or else skip instruction.
            if (moi == 0) {
                strcat(buf, "mul");
                strchara(buf, ' ');
                strcat(buf, regnames[destreg]);
                strcat(buf, ", ");
                strcat(buf, regnames[psrcreg]);
                strcat(buf, ", ");
                strcat(buf, soii ? tempbuf : regnames[ssrcreg]);
            } else {
                strcat(buf, "muls");
                strchara(buf, ' ');
                strcat(buf, regnames[destreg]);
                strcat(buf, ", ");
                strcat(buf, regnames[psrcreg]);
                strcat(buf, ", ");
                strcat(buf, soii ? tempbuf : regnames[ssrcreg]);
            }
            break;
        case 15: // mulh or mulx
            // check if core is multiply capable or else skip instruction.
            if (moi == 0) {
                strcat(buf, "mulh");
                strchara(buf, ' ');
                strcat(buf, regnames[destreg]);
                strcat(buf, ", ");
                strcat(buf, regnames[psrcreg]);
                strcat(buf, ", ");
                strcat(buf, soii ? tempbuf : regnames[ssrcreg]);
            } else {
                strcat(buf, "mulx");
                strchara(buf, ' ');
                strcat(buf, regnames[destreg]);
                strcat(buf, ", ");
                strcat(buf, regnames[psrcreg]);
                strcat(buf, ", ");
                strcat(buf, soii ? tempbuf : regnames[ssrcreg]);
            }
            break;
        case 1: // jmp...
            strcat(buf, "j");
            
            if (sync) {
                strchara(buf, 'y');
            }

            if (condindex == 0x00) {
                if (!sync) {
                    strcat(buf, jmpnames[0x00]);
                }
            } else {
                strcat(buf, jmpnames[condindex]);
            }

            strchara(buf, ' ');
            strcat(buf, regnames[destreg]);
            strcat(buf, ", ");
            strcat(buf, soii ? tempbuf : regnames[ssrcreg]);
            break;
        case 2: // ld
            strcat(buf, "ld");
            strchara(buf, ' ');
            strcat(buf, regnames[destreg]);
            strcat(buf, ", ");
            strcat(buf, regnames[psrcreg]);
            strcat(buf, ", ");
            strcat(buf, soii ? tempbuf : regnames[ssrcreg]);
            break;
        case 10: // st
            strcat(buf, "st");
            strchara(buf, ' ');
            strcat(buf, regnames[destreg]);
            strcat(buf, ", ");
            strcat(buf, regnames[psrcreg]);
            strcat(buf, ", ");
            strcat(buf, soii ? tempbuf : regnames[ssrcreg]);
            break;
        case 3: // exh
            strcat(buf, "exh");
            strchara(buf, ' ');
            strcat(buf, regnames[destreg]);
            strcat(buf, ", ");
            strcat(buf, regnames[psrcreg]);
            strcat(buf, ", ");
            strcat(buf, soii ? tempbuf : regnames[ssrcreg]);
            break;
        case 0: // mov
            strcat(buf, "mov");
            if (moi) {
                strchara(buf, 'f');
            }
            strchara(buf, ' ');
            strcat(buf, regnames[destreg]);
            strcat(buf, ", ");
            strcat(buf, regnames[psrcreg]);
            strcat(buf, ", ");
            strcat(buf, soii ? tempbuf : regnames[ssrcreg]);
            break;
    }
    return buf;
}