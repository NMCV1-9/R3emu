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
    VM_word result = VM_sub(3, 1, &flags);
    if (result != 2) {return 50;}
    if (VM_getflag(flags, 0) != 0) {return 10;}
    if (VM_getflag(flags, 1) != 0) {return 20;}
    if (VM_getflag(flags, 2) != 0) {return 30;}
    if (VM_getflag(flags, 3) != 0) {return 40;}

    /*
    test 1
    flags:
        - 0
        - 0
        - 1
        - 0
    */
    result = VM_add(2, (~1)+1, &flags);
    if (result != 1) {return 51;}
    if (VM_getflag(flags, 0) != 0) {return 11;}
    if (VM_getflag(flags, 1) != 0) {return 21;}
    if (VM_getflag(flags, 2) != 1) {return 31;}
    if (VM_getflag(flags, 3) != 0) {return 41;}

    /*
    test 2
    flags:
        - 0
        - 1
        - 0
        - 0
    */
    result = VM_add(2, (~4)+1, &flags);
    if (result != 65534) {return 52;}
    if (VM_getflag(flags, 0) != 0) {return 12;}
    if (VM_getflag(flags, 1) != 1) {return 22;}
    if (VM_getflag(flags, 2) != 0) {return 32;}
    if (VM_getflag(flags, 3) != 0) {return 42;}

    /*
    test 3
    flags:
        - 1
        - 0
        - 1
        - 0
    */
    result = VM_add(1, 65535, &flags);
    if (result != 0) {return 53;}
    if (VM_getflag(flags, 0) != 1) {return 13;}
    if (VM_getflag(flags, 1) != 0) {return 23;}
    if (VM_getflag(flags, 2) != 1) {return 33;}
    if (VM_getflag(flags, 3) != 0) {return 43;}

    /*
    test 4
    flags:
        - 0
        - 1
        - 1
        - 0
    */
    result = VM_sub(1, 4, &flags);
    if (result != 0b1111111111111101) {return 54;}
    if (VM_getflag(flags, 0) != 0) {return 14;}
    if (VM_getflag(flags, 1) != 1) {return 24;}
    if (VM_getflag(flags, 2) != 1) {return 34;}
    if (VM_getflag(flags, 3) != 0) {return 44;}
    return 0;
}