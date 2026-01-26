compile:
	meson compile -C build
clean:
	-rm -rf build
	-rm -rf subprojects
prepare: clean
	mkdir subprojects
	meson wrap install sdl2
	meson setup build
test:
	meson test -C build
buildtests:
	cd tests;lua buildtests.lua
	