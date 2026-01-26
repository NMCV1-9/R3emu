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
%eval term_pix term_base 0x6F +

%define term_width 12         ; * Terminal size in characters. Different demos
%define term_height 8         ;   take this into account to varying extents;
                              ;   some work well on screens of different size,
                              ;   some do not.
%define nlchar 10             ; * Use ASCII '\n' as the newline character.

%define return_addr_reg r31
%define stack_pointer r30

%macro call thing
    push return_addr_reg
    jmp return_addr_reg, thing
%endmacro
%macro ret
    mov r26, return_addr_reg
    pop return_addr_reg
    jmp r26
%endmacro
%macro push thing
    subs stack_pointer, 1
    st thing, stack_pointer
%endmacro
%macro pop thing
    ld thing, stack_pointer
    adds stack_pointer, 1
%endmacro
%macro genpos x, y, out
    mov out, 0x00
    shl out, y, 8
    add out, x
%endmacro
%macro setpix x, y
    push r20
    genpos x, y, r20
    st r20, term_pix
    pop r20
%endmacro
%macro pow x, y
        push r12
        test y, y
        jnz .powcontinue
        mov x, 0x00
        jmp .powdone
    .powcontinue:
        mov r12, x
    .powloop:
        sub y, 1
        test y, y
        jz .powdone
        mul x, x, r12
        jmp .powloop
    .powdone:
        pop r12
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

delay:
    sub r1, 1
    jnz delay
    ret

waitforkey:
    ld r1, term_input
    test r1, r1
    jz waitforkey
    ret

; r1 should contain depth
; r2 should contain position, x
; r3 should contain position, y
cube:
    test r1, r1
    jz .gen

    push r1
    push r2
    push r3
    push r5

    mov r5, 3
    mov r6, r1
    pow r5, r6
    sub r1, 1 ; decrease depth

    call cube ; top
    add r2, r5
    call cube
    add r2, r5
    call cube

    add r3, r5 ; middle right
    call cube

    add r3, r5 ; bottom
    call cube
    sub r2, r5
    call cube
    sub r2, r5
    call cube

    sub r3, r5 ; middle left
    call cube

    pop r5
    pop r3
    pop r2
    pop r1

    jmp .done
.gen:
    setpix r2, r3 ; top
    add r2, 1
    setpix r2, r3
    add r2, 1
    setpix r2, r3

    add r3, 1
    setpix r2, r3 ; middle right

    add r3, 1 ; bottom
    setpix r2, r3
    sub r2, 1
    setpix r2, r3
    sub r2, 1
    setpix r2, r3

    sub r3, 1
    setpix r2, r3 ; middle left
    sub r3, 1
.done:
    ret

main:
    mov stack_pointer,8191
    mov r1, 0xA
    st r1, term_colour
    call term_clear

    mov r1, 3
    mov r2, 0
    mov r3, 0
    call cube
    
done:
    call waitforkey
    hlt