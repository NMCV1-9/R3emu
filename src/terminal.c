/*
Written by Justus Wolff in very late 2025.
*/

#include "common.h"
#include "font8x8_basic.h"

#define CONF_charsnh 12
#define CONF_charsnv 8
#define CONF_haspixplot 1
#define CONF_charsam CONF_charsnh*CONF_charsnv // dont edit
#define CONF_pixam (8*CONF_charsnh)*(8*CONF_charsnv) // dont edit

/* color table
0 	#000000 	black
1 	#AA0000 	dark red
2 	#00AA00 	dark green
3 	#AAAA00 	dark yellow
4 	#0000AA 	dark blue
5 	#AA00AA 	dark magenta
6 	#00AAAA 	dark cyan
7 	#AAAAAA 	light grey
8 	#555555 	dark grey
9 	#FF5555 	light red
10 	#55FF55 	light green
11 	#FFFF55 	light yellow
12 	#5555FF 	light blue
13 	#FF55FF 	light magenta
14 	#55FFFF 	light cyan
15 	#FFFFFF 	white
*/

typedef uint8_t VM_pixel;
VM_pixel VM_colortable[16][3] = {
    {0x00, 0x00, 0x00}, // black
    {0x00, 0x00, 0xAA}, // dark blue
    {0x00, 0xAA, 0x00}, // dark green
    {0x00, 0xAA, 0xAA}, // dark cyan
    {0xAA, 0x00, 0x00}, // dark red
    {0xAA, 0x00, 0xAA}, // dark magenta
    {0xAA, 0xAA, 0x00}, // dark yellow
    {0xAA, 0xAA, 0xAA}, // light grey
    {0x55, 0x55, 0x55}, // dark grey
    {0x55, 0x55, 0xFF}, // light blue
    {0x55, 0xFF, 0x55}, // light green
    {0x55, 0xFF, 0xFF}, // light cyan
    {0xFF, 0x55, 0x55}, // light red
    {0xFF, 0x55, 0xFF}, // light magenta
    {0xFF, 0xFF, 0x55}, // light yellow
    {0xFF, 0xFF, 0xFF}, // white
};

typedef struct {
    uint8_t pixbuf[CONF_pixam];
    uint8_t colors;
    uint16_t hrange;
    uint16_t vrange;
    uint16_t cursor;
    uint8_t nlchar;
    uint32_t scrollmask;
    uint32_t char0even;
    uint32_t char0odd;
} VM_term;

VM_term VM_newterm() {
    VM_term out;
    for (uint32_t i=0;i<CONF_pixam;i++) {
        out.pixbuf[i] = 0;
    }
    out.nlchar = '\n';
    return out;
}
void VM_setrawpix(VM_term* term, uint32_t x, uint32_t y, SDL_Renderer* renderer, uint8_t color) {
    term->pixbuf[x+(y*CONF_charsnh*8)] = color;
	//SDL_SetRenderDrawColor(renderer, color[0], color[1], color[2], 255);
    //SDL_RenderDrawPoint(renderer, x, y);
}
void VM_setpix(VM_term* term, uint32_t x, uint32_t y, uint8_t colorindex, SDL_Renderer* renderer) {
    //uint8_t r,g,b;
    //r = VM_colortable[colorindex][0];
    //g = VM_colortable[colorindex][1];
    //b = VM_colortable[colorindex][2];
    VM_setrawpix(term, x, y, renderer, colorindex);
}
void VM_setchar(VM_term* term, uint8_t fcolor, uint8_t bcolor, uint8_t charindex, uint8_t column, uint8_t row, SDL_Renderer* renderer) {
    if (charindex > 127) {
        charindex = 0x00;
    }
    for (uint8_t y=0;y<8;y++) {
        for (uint8_t x=0;x<8;x++) {
            VM_setpix(term, x+(column*8), y+(row*8), font8x8_basic[charindex][y]>>x & 1 ? fcolor : bcolor, renderer);
        }
    }
}
void VM_copypix(VM_term* term, uint32_t sx, uint32_t sy, uint32_t dx, uint32_t dy, SDL_Renderer* renderer) {
    VM_setpix(term, dx, dy, term->pixbuf[sx+(sy*(8*CONF_charsnh))], renderer);
}
void VM_copycharpix(VM_term* term, uint8_t sx, uint8_t sy, uint8_t dx, uint8_t dy, SDL_Renderer* renderer) {
    sy *= 8;
    sx *= 8;
    dx *= 8;
    dy *= 8;
    for (uint8_t y=0;y<8;y++) {
        for (uint8_t x=0;x<8;x++) {
            VM_copypix(term, sx+x, sy+y, dx+x, dy+y, renderer);
        }
    }
}


