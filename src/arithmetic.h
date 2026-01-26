#include <stdint.h>
#include "common.h"

uint8_t VM_getflag(VM_flags flags, uint8_t index);
void VM_setflag(VM_flags* flags, uint8_t index, uint8_t state);

VM_word VM_aluwordlimit(VM_word x);

VM_word VM_add(VM_word a, VM_word b, VM_flags* flags);
VM_word VM_adc(VM_word a, VM_word b, VM_flags* flags, uint8_t carryin);
VM_word VM_sub(VM_word a, VM_word b, VM_flags* flags);
VM_word VM_sbb(VM_word a, VM_word b, VM_flags* flags, uint8_t carryin);
VM_word VM_mulh(VM_word a, VM_word b);
VM_word VM_muls(VM_word a, VM_word b);
VM_word VM_mulx(VM_word a, VM_word b);
VM_word VM_mul(VM_word a, VM_word b);
VM_word VM_shl(VM_word target, VM_word pos, VM_flags* flags);
VM_word VM_shr(VM_word target, VM_word pos, VM_flags* flags);
VM_word VM_and(VM_word a, VM_word b, VM_flags* flags);
VM_word VM_or(VM_word a, VM_word b, VM_flags* flags);
VM_word VM_xor(VM_word a, VM_word b, VM_flags* flags);
void VM_generatecondtable(VM_flags flags, VM_word* buf);