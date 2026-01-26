#pragma once

#include "memory.h"
#include <stdint.h>

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

VM_vminstance VM_newinstance(uint8_t memsize, uint8_t coreamount, uint8_t* coretypes);
void VM_delinstance(VM_vminstance inst);
void VM_instcycle(VM_vminstance* _inst);