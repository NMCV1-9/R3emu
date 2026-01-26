/*
Written by Justus Wolff in very late 2025.
*/

#include "common.h"


/*
flag indexes:
0: Zf		zero flag
1: Sf		sign flag
2: Cf		carry flag
3: Of		overflow flag

*/
uint8_t VM_getflag(VM_flags flags, uint8_t index) {
	return (flags >> index) & 0x1;
}
void VM_setflag(VM_flags* flags, uint8_t index, uint8_t state) {
	if (state) {
		*flags |= 1 << index; 
	} else {
		*flags &= ~(1 << index);
	}
}

VM_word VM_aluwordlimit(VM_word x) {
	return x & 0xFFFF;
} 
void VM_updflags(VM_word x, VM_flags* flags) {
	if (flags == 0x00) {return;}
    VM_setflag(flags, 0, VM_aluwordlimit(x)==VM_nullword); // zero flag
    VM_setflag(flags, 1, VM_aluwordlimit(x) >> 15); // sign flag
    VM_setflag(flags, 2, x > 0xFFFF); // carry flag
}
VM_word VM_addcommon(VM_word a, VM_word b, VM_flags* flags, uint8_t carryin) {
    patchword(&a);patchword(&b);
    a = VM_aluwordlimit(a);
    b = VM_aluwordlimit(b);

    //std::cout << "DBG: " << a << ":" << b << std::endl;

    VM_word out = ((uint16_t)a+(uint16_t)b+(uint16_t)carryin);
    patchword(&out);
    if (flags) {
        VM_setflag(flags, 0, VM_aluwordlimit(out)==VM_nullword); // zero flag
        VM_setflag(flags, 1, VM_aluwordlimit(out) >> 15); // sign flag
        VM_setflag(flags, 2, out > 0xFFFF); // carry flag
        VM_setflag(flags, 3, ((a >> 15) & 1) == ((b >> 15) & 1) && (VM_aluwordlimit(out) >> 15) != ((a >> 15) & 1)); // overflow flag
    }
    out = VM_aluwordlimit(out);
    return out;
}
VM_word VM_add(VM_word a, VM_word b, VM_flags* flags) {
    return VM_addcommon(a, b, flags, 0);
}
VM_word VM_adc(VM_word a, VM_word b, VM_flags* flags, uint8_t carryin) {
    return VM_addcommon(a, b, flags, carryin);
}
VM_word VM_sub(VM_word a, VM_word b, VM_flags* flags) {
    VM_word out = VM_addcommon(a, ~b, flags, 1);
	if (flags) {
		VM_setflag(flags, 2, !VM_getflag(*flags, 2));
	}
	return out;
}
VM_word VM_sbb(VM_word a, VM_word b, VM_flags* flags, uint8_t carryin) {
    VM_word out = VM_addcommon(a, ~b, flags, !carryin);
	if (flags) {
		VM_setflag(flags, 2, !VM_getflag(*flags, 2));
	}
	return out;
}
VM_word VM_mulh(VM_word a, VM_word b) {
	patchword(&a);patchword(&b);
	a = VM_aluwordlimit(a);
	b = VM_aluwordlimit(b);

	VM_word out = a * b;

	return ((VM_word)out)>>16;
}
VM_word VM_muls(VM_word a, VM_word b) {
	patchword(&a);patchword(&b);
	a = VM_aluwordlimit(a);
	b = VM_aluwordlimit(b);

	int32_t out = (int32_t)((int16_t)a) * (int32_t)((int16_t)b);

	return ((VM_word)out)>>16;
}
VM_word VM_mulx(VM_word a, VM_word b) {
	patchword(&a);patchword(&b);
	a = VM_aluwordlimit(a);
	b = VM_aluwordlimit(b);

	int32_t out = (uint32_t)a * (int32_t)((int16_t)b);

	return ((VM_word)out)>>16;
}
VM_word VM_mul(VM_word a, VM_word b) {
	patchword(&a);patchword(&b);
	a = VM_aluwordlimit(a);
	b = VM_aluwordlimit(b);

	VM_word out = (uint16_t)a * (uint16_t)b;
	out &= 0xFFFF;

	return out;
}
VM_word VM_shl(VM_word target, VM_word pos, VM_flags* flags) {
	patchword(&target);patchword(&pos);

	pos &= 0b1111;
	VM_word out = target << pos;
	VM_aluwordlimit(out);
	patchword(&out);

	if (flags) {
		VM_setflag(flags, 0, out==0); // zero flag
		VM_setflag(flags, 1, VM_aluwordlimit(out) >> 15); // sign flag
	}

	return out;
}
VM_word VM_shr(VM_word target, VM_word pos, VM_flags* flags) {
	patchword(&target);patchword(&pos);

	pos &= 0b1111;
	VM_aluwordlimit(target);
	VM_word out = target >> pos;
	patchword(&out);

	if (flags) {
		VM_setflag(flags, 0, out==0); // zero flag
		VM_setflag(flags, 1, VM_aluwordlimit(out) >> 15); // sign flag
	}

	return out;
}
VM_word VM_and(VM_word a, VM_word b, VM_flags* flags) {
	patchword(&a);patchword(&b);

	VM_word out = a & b;
	patchword(&out);

	if (flags) {
		VM_setflag(flags, 2, 0); // carry flag
		VM_setflag(flags, 0, out==0); // zero flag
		VM_setflag(flags, 1, VM_aluwordlimit(out) >> 15); // sign flag
	}

	return out;
}
VM_word VM_or(VM_word a, VM_word b, VM_flags* flags) {
	patchword(&a);patchword(&b);

	VM_word out = a | b;
	patchword(&out);

	if (flags) {
		VM_setflag(flags, 2, 0); // carry flag
		VM_setflag(flags, 0, out==0); // zero flag
		VM_setflag(flags, 1, VM_aluwordlimit(out) >> 15); // sign flag
	}

	return out;
}
VM_word VM_xor(VM_word a, VM_word b, VM_flags* flags) {
	patchword(&a);patchword(&b);

	VM_word out = a ^ b;
	patchword(&out);

	if (flags) {
		VM_setflag(flags, 2, 0); // carry flag
		VM_setflag(flags, 0, out==0); // zero flag
		VM_setflag(flags, 1, VM_aluwordlimit(out) >> 15); // sign flag
	}

	return out;
}
void VM_generatecondtable(VM_flags flags, VM_word* buf) {
	uint8_t cf,zf,sf,of;
	cf = VM_getflag(flags, 2);
	zf = VM_getflag(flags, 0);
	sf = VM_getflag(flags, 1);
	of = VM_getflag(flags, 3);

	buf[0] = 1; // true
	buf[1] = cf | zf;
	buf[2] = sf ^ of;
	buf[3] = zf | (sf ^ of);
	buf[4] = sf;
	buf[5] = zf;
	buf[6] = of;
	buf[7] = cf;

	// inverted conditions
	for (uint8_t i=8;i<16;i++) {
		buf[i] = !buf[i-8];
	}
}
