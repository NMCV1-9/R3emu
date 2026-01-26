An (full) R3 emulator...
Why did I start making this again? Oh right cuz its 1 AM in the morning

How to use:

to compile the emulator you need the meson build system, make and a C++ compiler. that will also automatically download the dependencies (SDL2)

first make prepare
then make compile

the emulator can be found in build/R3emu
NOTE: for windows, you need to swap out the meson.build file with the meson_win.build file!



usage of the executable: R3emu input.bin

how you can get these bin files? simple: you assemble tptasm files. there is an tpt assembler shipped with the
repo under tests/tptasm.lua but for that you need to have lua installed on your system.
then you can call tptasm from any file and use it as usual to output a bin file.

there are also a few tpt assembly files under tests aswell as their bin equivalents.
these include:

demo				the original demo running on the R3
colortest			an testing ROM I made to test the display colors
demodism			same as demo, except an disassembled version from an disassembler I wrote
pixplot				serpinski carpet. Do I have to say more?
test0				An basic testing ROM I made in the early hours in development
wordle				the wordle game from entropite with their C compiler. (go check that out! copy of that is under /tests/compiler)

NOTE: the emulator automatically closes the window as soon as the emulation finishes.
to configure the emulator like amount of memory, if the terminal has a pixel plotter or the FPS limiter, you must go into the source files.

here is a list of configurations and their filenames under src (you find these normally toward the top under #define's):

CONF_memrows					main.cpp					amount of memory rows that are emulated. NOTE about memory: the mirrored memory is not emulated.
CONF_coreamount					main.cpp					amount of cores the emulator emulates. can go up to 50 by standard. check main.cpp to see how its done. all cores are multiply capable by default.
CONF_memdump					main.cpp					if 1, dumps memory after emulation and an disassembled version
CONF_targetfps					main.cpp					amount of fps the emulator tries to reach.
CONF_fpslimiter					main.cpp					if 1, activates the fps limiter. see: CONF_targetfps
CONF_updxframe					main.cpp					specifies how many frames to wait until the window (frame) updates. can be used to gain a bit more speed
CONF_maketracedump				cores.c						if 1, dumps the instruction trace after emulation and a disassembled version.
CONF_tracesize					cores.c						how much space gets reserved for the trace dump.
CONF_charsnh					terminal.c					horizontal char size of the terminal.
CONF_charsnv					terminal.c					vertical equivalent of CONF_charsnh.
CONF_haspixelplot				terminal.c					self explenatory, if 1 the terminal has an pixel plotter.

NOTE: for the configurations to take affect, you need to recompile after making a change!

I think I explained everything now. For questions: technik_hea on DC

