/*
Written by Justus Wolff in very late 2025.
*/

#include <stdint.h>

typedef uint32_t VM_word;
typedef int32_t VM_sword;
typedef VM_word VM_registers[31];
typedef uint16_t VM_aluword;
#define VM_nullword 0x00000000

// config
#define VM_allowsmul 1
// if S units may be able to multiply. they can either always multiply or never.
// config end

void patchword(VM_word* word);
void writereg(VM_registers* regs, uint8_t index, VM_word word);
VM_word readreg(VM_registers* regs, uint8_t index);


