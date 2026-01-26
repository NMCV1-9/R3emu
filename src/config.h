#pragma once


// emulator needs to be recompiled IF you change these for things to have an effect.

#define VM_allowsmul 1
// if S units may be able to multiply. they can either always multiply or never.

#define CONF_maketracedump 0
#define CONF_tracesize 100000


// main
#define CONF_memrows 64 // amount of memory rows get emulated.
#define CONF_coreamount 10 // amount of cores the emulator emulates. can go up to an max of 50 by standard, check the main function. all cores are multiply capable by standard.
#define CONF_memdump 0 // if 1, dumps the memory after emulation
#define CONF_targetfps 60 // target fps. multiply this by core amount and you got your target ips.
#define CONF_fpslimiter 1 // if set to 1, tries to limit the fps to targetfps.
#define CONF_updxframes 10 // only updates the SDL window every <this value>th frame.


// memory
#define VM_rowsize 128
