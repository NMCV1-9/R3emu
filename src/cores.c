/*
Written by Justus Wolff in very late 2025.
*/

#include "arithmetic.c"
#include "memory.c"

#define CONF_maketracedump 0
#define CONF_tracesize 100000

typedef struct {
    uint8_t cores[50];
    /*
    types:
        - 0: f
        - 1: s
        - 2: m
    */
    uint8_t coreamount;
    VM_memory memory;
    VM_registers regs;
    VM_flags flags;
    VM_word IP;
    uint8_t halted;

    uint16_t sch_addr; // schedule addr
    uint8_t sch_mode; // schedule mode
    uint8_t sch_reg; // schedule reg

    VM_word backtrace[CONF_tracesize];
    VM_word backtraceaddrs[CONF_tracesize];
    VM_word backtraceop[CONF_tracesize][3];
    uint64_t tracesize;
} VM_vminstance;

VM_vminstance VM_newinstance(uint8_t memsize, uint8_t coreamount, uint8_t* coretypes) {
    VM_vminstance out;

    out.memory = VM_newmemory(memsize);
    out.coreamount = coreamount;
    for (uint8_t i=0;i<coreamount;i++) {
        out.cores[i] = coretypes[i];
    }
    out.IP = 0;
    out.halted = 0;
    out.tracesize = 0;

    return out;
}
void VM_delinstance(VM_vminstance inst) {
    free(inst.memory.content);
}
void VM_execinstruction(VM_vminstance* _inst, uint8_t coreindex) {
    VM_vminstance inst = *_inst;
    // fetch instruction from memory
    if (inst.IP > VM_getsize(inst.memory.rows)) {inst.IP = VM_nullword;} // reset IP

    VM_word instruction = VM_memread(inst.memory, inst.IP);

    if (CONF_maketracedump) {
        inst.backtrace[inst.tracesize] = instruction;
        inst.backtraceaddrs[inst.tracesize] = inst.IP;
    }

    uint8_t moi = instruction >> 31; // msb operation index
    uint8_t soii = (instruction >> 30) & 0x1; // second operand is an immediate value
    uint8_t destreg = (instruction >> 25) & 0b11111; // dest register index
    uint8_t psrcreg = (instruction >> 20) & 0b11111; // pr. src register index
    uint8_t loi = (instruction >> 16) & 0b1111; // lsb operation index
    uint16_t ssrcreg = instruction & 0b1111111111111111; // sec. src register index (immediate value)
	uint16_t rawssrcreg = ssrcreg;

    if (!soii) {
        ssrcreg = readreg(&inst.regs, ssrcreg);
    }

    patchword((VM_word*)&ssrcreg);

    if (CONF_maketracedump) {
        inst.backtraceop[inst.tracesize][0] = destreg;
        inst.backtraceop[inst.tracesize][1] = readreg(&inst.regs, psrcreg);
        inst.backtraceop[inst.tracesize++][2] = ssrcreg;
    }

    // jmp conditions
    uint8_t sync = !(psrcreg >> 4);
    uint8_t condindex = psrcreg & 0b1111;



    VM_registers* regs = &inst.regs;
    VM_flags* flags = &inst.flags;
    uint8_t coretype = inst.cores[coreindex];
    uint8_t skipins = 0;

    switch (loi) {
        case 4: // sub
            writereg(regs, destreg, VM_sub(ssrcreg, readreg(regs, psrcreg), moi ? flags : 0x00));
            break;
        case 5: // sbb
            writereg(regs, destreg, VM_sbb(ssrcreg, readreg(regs, psrcreg), moi ? flags : 0x00, VM_getflag(*flags, 2)));
            break;
        case 6: // add
            writereg(regs, destreg, VM_add(readreg(regs, psrcreg), ssrcreg, moi ? flags : 0x00));
            break;
        case 7: // adc
            writereg(regs, destreg, VM_adc(readreg(regs, psrcreg), ssrcreg, moi ? flags : 0x00, VM_getflag(*flags, 2)));
            break;
        case 8: // xor
            writereg(regs, destreg, VM_xor(readreg(regs, psrcreg), ssrcreg, moi ? flags : 0x00));
            break;
        case 9: // or
            writereg(regs, destreg, VM_or(readreg(regs, psrcreg), ssrcreg, moi ? flags : 0x00));
            break;
        case 11: // bitshift
            if ((rawssrcreg >> 15) == 0) {
                writereg(regs, destreg, VM_shl(readreg(regs, psrcreg), (ssrcreg) & 0b1111, moi ? flags : 0x00));
            } else {
                writereg(regs, destreg, VM_shr(readreg(regs, psrcreg), (ssrcreg) & 0b1111, moi ? flags : 0x00));
            }
            break;
        case 12: // and
            writereg(regs, destreg, VM_and(readreg(regs, psrcreg), ssrcreg, moi ? flags : 0x00));
            break;
        case 13: // hlt
            inst.halted = 1;
            break;
        case 14: // mul or muls
            // check if core is multiply capable or else skip instruction.
            if (moi == 0) {
                if ((coretype == 1 && VM_allowsmul) || coretype == 2) {
                    writereg(regs, destreg, VM_mul(readreg(regs, psrcreg), ssrcreg));
                } else {
                    skipins = 1;
                }
            } else {
                if ((coretype == 1 && VM_allowsmul) || coretype == 2) {
                    writereg(regs, destreg, VM_muls(readreg(regs, psrcreg), ssrcreg));
                } else {
                    skipins = 1;
                }
            }
            break;
        case 15: // mulh or mulx
            // check if core is multiply capable or else skip instruction.
            if (moi == 0) {
                if ((coretype == 1 && VM_allowsmul) || coretype == 2) {
                    writereg(regs, destreg, VM_mulh(readreg(regs, psrcreg), ssrcreg));
                } else {
                    skipins = 1;
                }
            } else {
                if ((coretype == 1 && VM_allowsmul) || coretype == 2) {
                    writereg(regs, destreg, VM_mulx(readreg(regs, psrcreg), ssrcreg));
                } else {
                    skipins = 1;
                }
            }
            break;
        case 1: // jmp...
            VM_word condtable[16];
            VM_generatecondtable(inst.flags, condtable);
            if (condtable[condindex]) {
                skipins = 1;
                if (!sync || (sync && coreindex != (inst.coreamount))) {
                    writereg(regs, destreg, inst.IP+1);
                    inst.IP = ssrcreg;
                }
            }
            break;
        case 2: // ld
            inst.sch_mode = 0x0;
            inst.sch_addr = VM_aluwordlimit(readreg(&inst.regs, psrcreg))+VM_aluwordlimit(ssrcreg);
            inst.sch_reg = destreg;
            break;
        case 10: // st
            inst.sch_mode = 0x1;
            inst.sch_addr = VM_aluwordlimit(readreg(&inst.regs, psrcreg))+VM_aluwordlimit(ssrcreg);
            inst.sch_reg = destreg;
            break;
        default: // mov/exh
            if (loi == 0) {
                VM_word newval = ((readreg(&inst.regs, psrcreg)>>16)<<16)|(ssrcreg&0xFFFF);
                patchword(&newval);
                writereg(regs, destreg, newval);
                if (moi) {
                    VM_setflag(flags, 0, newval==0x00);
                    VM_setflag(flags, 1, newval>>31);
                    VM_setflag(flags, 2, 0);
                }
                break;
            } else { // exh
                VM_word newval2 = (readreg(&inst.regs, psrcreg)<<16)|(ssrcreg>>16);
                patchword(&newval2);
                writereg(regs, destreg, newval2);
                if (moi) {
                    VM_setflag(flags, 0, newval2==0x00);
                    VM_setflag(flags, 1, newval2>>31);
                    VM_setflag(flags, 2, 0);
                }
            }
            break;
        
    }

    if (!skipins) {
        inst.IP ++;
    }

    *_inst = inst;
}
void VM_handleschmem(VM_vminstance* inst) {
    if (inst->sch_mode == 0x0) { // memread
        writereg(&inst->regs, inst->sch_reg, VM_memread(inst->memory, inst->sch_addr));
    }
    if (inst->sch_mode == 0x1) { // memwrite
        VM_memwrite(inst->memory, inst->sch_addr, readreg(&inst->regs, inst->sch_reg));
    }
    inst->sch_mode = 0x2;
}
void VM_instcycle(VM_vminstance* _inst) {
    VM_vminstance inst = *_inst;

    for (uint8_t i=0;i<inst.coreamount;i++) {
        VM_handleschmem(&inst);
        if (inst.halted) {break;}
        VM_execinstruction(&inst, i);
    }
    VM_handleschmem(&inst);

    *_inst = inst;
}

