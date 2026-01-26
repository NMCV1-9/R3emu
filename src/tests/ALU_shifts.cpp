#include <iostream>
#include "../arithmetic.c"
#include "../common.c"

/*
codes:
0 - OK
1x - flag 0 failure
2x - flag 1 failure
3x - flag 2 failure
4x - flag 3 failure
5x - output failure
*/

int main() {
    VM_flags flags;

    /*
    test 0
    flags:
        - 0
        - 0
        - 0
        - 0
    */
    VM_word result = VM_shl(1, 1, &flags);
    if (result != 2) {return 50;}
    if (VM_getflag(flags, 0) != 0) {return 10;}
    if (VM_getflag(flags, 1) != 0) {return 20;}
    if (VM_getflag(flags, 2) != 0) {return 30;}
    if (VM_getflag(flags, 3) != 0) {return 40;}

    /*
    test 1
    flags:
        - 1
        - 0
        - 1
        - 0
    */
    result = VM_shl(0x5, 2, &flags);
    if (result != 20) {return 51;}
    if (VM_getflag(flags, 0) != 0) {return 11;}
    if (VM_getflag(flags, 1) != 0) {return 21;}
    if (VM_getflag(flags, 2) != 0) {return 31;}
    if (VM_getflag(flags, 3) != 0) {return 41;}

    /*
    test 2
    flags:
        - 0
        - 0
        - 1
        - 1
    */
    result = VM_shr(6, 2, &flags);
    if (result != 1) {return 52;}
    if (VM_getflag(flags, 0) != 0) {return 12;}
    if (VM_getflag(flags, 1) != 0) {return 22;}
    if (VM_getflag(flags, 2) != 1) {return 32;}
    if (VM_getflag(flags, 3) != 0) {return 42;}
    return 0;
}