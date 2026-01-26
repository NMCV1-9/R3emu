typedef struct {
    char keycode;
} VM_keyboard;

char VM_getkey(VM_keyboard* keyboard);
VM_keyboard VM_newkeyboard();
void VM_registerkeypress(VM_keyboard* keyboard, char key);