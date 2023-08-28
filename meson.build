project('plazma', 'c', 'cpp', default_options: ['cpp_std=c++20', 'warning_level=3'])

liblzma_dep = dependency('liblzma')
thesauros_dep = dependency('thesauros')

plazma_lib = library('plazma', 
  'src/args.c',
  'src/coder.c',
  'src/file_io.c',
  'src/hardware.c',
  'src/main.c',
  'src/message.c',
  'src/suffix.c',
  'src/tuklib_cpucores.c',
  'src/tuklib_exit.c',
  'src/tuklib_physmem.c',
  'src/util.c',
  dependencies: [liblzma_dep],
)

plazma_dep = declare_dependency(
  include_directories: include_directories('include', 'src'),
  dependencies: [liblzma_dep, thesauros_dep],
  link_with: plazma_lib,
)

install_subdir(
  'include',
  install_dir: 'include',
)

pc = import('pkgconfig')
pc.generate(
  name: 'plazma',
  version: meson.project_version(),
  description: 'LZMA library wrapper',
)

subdir('tools')

if get_option('test')
  subdir('test')
endif
