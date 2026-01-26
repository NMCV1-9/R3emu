%include "common"

%define term_base 0x9F80      ; * Terminal is expected to be mapped to this
                              ;   address. Macros aliasing individual registers
                              ;   follow. See the R3 manual for the meaning and
                              ;   function of these registers.
                              ; * TODO: encode as metadata
%eval term_input  term_base 0x00 +
%eval term_raw    term_base 0x04 +
%eval term_single term_base 0x05 +
%eval term_term   term_base 0x35 +
%eval term_hrange term_base 0x42 +
%eval term_vrange term_base 0x43 +
%eval term_cursor term_base 0x44 +
%eval term_nlchar term_base 0x45 +
%eval term_colour term_base 0x46 +

%define term_width 12         ; * Terminal size in characters. Different demos
%define term_height 8         ;   take this into account to varying extents;
                              ;   some work well on screens of different size,
                              ;   some do not.
%define nlchar 10             ; * Use ASCII '\n' as the newline character.
%define lr r31

%macro call thing             ; * There is no hardware support for functions,
    jmp lr, thing             ;   so we roll our own call and ret primitives.
%endmacro                     ;   We store the current address in lr in call and
%macro ret                    ;   jump as normal, then we jump back to lr in
    jmp lr                    ;   ret. This means that functions that call
%endmacro  

jmp main

term_clear:
    st r0, term_cursor        ; * Move cursor to top left corner.
    mov r1, { term_width 1 - } ; * Encode the ordered closed range [0,
    shl r1, 5                  ;   term_width - 1] in terminal format, then pass
    st r1, term_hrange         ;   it to the terminal.
    mov r1, { term_height 1 - } ; * Encode the ordered closed range [0,
    shl r1, 5                   ;   term_height - 1] in terminal format, then
    st r1, term_vrange          ;   pass it to the terminal.
    mov r1, nlchar            ; * Configure newline character.
    st r1, term_nlchar
    mov r1, ' '
    mov r2, 0                 ; * Do as many full-row scrollprints as there are
.loop:                        ;   screen lines, clearing the screen.
    st r1, term_raw
    add r2, 1
    cmp r2, term_height
    jne .loop
    ret

term_print:
    ld r2, r1                 ; * Load character,
    add r1, 1                 ;   increment pointer.
    test r2, r2               ; * If the character terminates the string, exit.
    jz .done
    st r2, term_term          ; * Otherwise, print the character and go again.
    jmp term_print
.done:
    ret

msg_begin:
    dw "Color indexing test, R3 emu 2025. Press any key to start.", 0

msg_numbers:
    dw "0", 0, "1", 0, "2", 0, "3", 0, "4", 0, "5", 0, "6", 0, "7", 0, "8", 0, "9", 0, "A", 0, "B", 0, "C", 0, "D", 0, "E", 0, "F", 0

msg_menu:
    dw "Navigate with a and d to switch colors. Press any key to start.", 0

delay:
    sub r1, 1
    jnz delay
    ret

waitforkey:
    ld r1, term_input
    test r1, r1
    jz waitforkey
    ret

main:
    mov r1, 0xA
    st r1, term_colour
    call term_clear
    mov r1, msg_begin
    call term_print
    call waitforkey
    call term_clear

    mov r4, 0
mainloop:
    mov r3, r4
    shl r3, 4
    add r3, r4
    st r3, term_colour
    call term_clear

    mov r3, 0xA
    st r3, term_colour
    test r4, r4
    jz .skipcolset

    mov r3, r4
    shl r3, 4
    st r3, term_colour
.skipcolset:
    mul r1, r4, 2
    add r1, msg_numbers
    call term_print

    mov r1, 10
    call delay
    
    add r4, 1
    cmp r4, 16
    jne mainloop

mov r1, 0xA
st r1, term_colour
call term_clear
mov r1, msg_menu
call term_print
call waitforkey

mov r4, 0
menuloop:
    mov r3, r4
    shl r3, 4
    add r3, r4
    st r3, term_colour
    call term_clear

    mov r3, 0xA
    st r3, term_colour
    test r4, r4
    jz skipcolset2

    mov r3, r4
    shl r3, 4
    st r3, term_colour
skipcolset2:
    mul r1, r4, 2
    add r1, msg_numbers
    call term_print

    mov r1, 100
    call delay
    
    call waitforkey
    cmp r1, 'a'
    je decrease
    
    cmp r1, 'd'
    jne menuloop ; not a valid key

    cmp r4, 15 ; d handler
    je menuloop
    add r4, 1

    jmp menuloop

decrease:
    test r4, r4 ; a handler
    jz menuloop
    sub r4, 1

    jmp menuloop



