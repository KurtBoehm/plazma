project('plazma', 'cpp', default_options: ['cpp_std=c++20', 'warning_level=3'])

fmt_dep = dependency('fmt')
liblzma_dep = dependency('liblzma')
thesauros_dep = dependency('thesauros')

plazma_dep = declare_dependency(
  include_directories: include_directories('include'),
  dependencies: [fmt_dep, liblzma_dep, thesauros_dep],
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
