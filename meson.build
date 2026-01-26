
project(
  'R3emu',
  ['cpp', 'c'],
  version : '0.1',
  default_options : ['warning_level=3']
)

project_source_files = [
  'src/arithmetic.c',
  'src/common.c',
  'src/cores.c',
  'src/disassembler.c',
  'src/keyboard.c',
  'src/main.cpp',
  'src/memory.c',
  'src/terminal.c'
]

project_dependencies = [
  dependency('sdl2', fallback : ['sdl2', 'sdl2_dep']),
]

build_args = [
]


# ===================================================================

# ======
# Target
# ======

build_args += [
  '-DPROJECT_NAME=' + meson.project_name(),
  '-DPROJECT_VERSION=' + meson.project_version(),
]

project_target = executable(
  meson.project_name(),
  project_source_files,
  dependencies: project_dependencies,
  install : true,
  c_args : build_args,
)

test('basic', project_target)
t1 = executable('TEST_ALU_add', 'src/tests/ALU_add.cpp')
test('ALU_add', t1)
t2 = executable('TEST_ALU_sub', 'src/tests/ALU_sub.cpp')
test('ALU_sub', t2)
t3 = executable('TEST_ALU_shifts', 'src/tests/ALU_shifts.cpp')
test('ALU_shifts', t3)
