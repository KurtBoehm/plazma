project(
  'argparse',
  'cpp',
  version: '3.0',
  license: 'MIT',
)

install_headers(
  ['include/argparse/argparse.hpp'],
  subdir: 'argparse',
)

argparse_dep = declare_dependency(
  include_directories: include_directories('include'),
)
