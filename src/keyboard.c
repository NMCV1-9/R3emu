/*
Written by Justus Wolff in very late 2025.
*/

#include "common.h"
#include "keyboard.h"
void VM_registerkeypress(VM_keyboard* keyboard, char key) {
    keyboard->keycode = key;
}
VM_keyboard VM_newkeyboard() {
    VM_keyboard out;
    out.keycode = 0x00;
    return out;
}
char VM_getkey(VM_keyboard* keyboard) {
    char temp = keyboard->keycode;
    keyboard->keycode = 0x00;
    return temp;
}