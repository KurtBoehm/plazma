project('plazma', 'cpp', default_options: ['cpp_std=c++23', 'warning_level=3'])

liblzma_sub = subproject('liblzma', default_options: ['default_library=static'])
liblzma_dep = liblzma_sub.get_variable('lzma_dep')
fmt_sub = subproject('fmt', default_options: ['default_library=static'])
fmt_dep = fmt_sub.get_variable('fmt_dep')
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

if not meson.is_subproject()
  subdir('tools')
endif

if get_option('test')
  subdir('test')
endif
