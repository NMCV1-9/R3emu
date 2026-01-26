/*
Written by Justus Wolff in very late 2025.
*/

#include "common.h"

void patchword(VM_word* word) {
	VM_word _word = *word;
	if (_word == 0x00000000 || _word == 0x40000000 || _word == 0x80000000 || _word == 0xC0000000) {
		*word = 0x00000000;
	}
}
void writereg(VM_registers* regs, uint8_t index, VM_word word) {
	if (index > 31 || index == 0) {
		return;
	}
	patchword(&word);
	(*regs)[index-1] = word;
}
VM_word readreg(VM_registers* regs, uint8_t index) {
	if (index > 31 || index == 0) {
		return 0x00000000;
	}
	return (*regs)[index-1];
}


