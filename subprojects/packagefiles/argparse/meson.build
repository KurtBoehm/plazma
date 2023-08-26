project(
  'argparse',
  'cpp',
  version: '2.9',
  license: 'MIT',
)

install_headers(
  ['include/argparse/argparse.hpp'],
  subdir: 'argparse',
)

argparse_dep = declare_dependency(
  include_directories: include_directories('include'),
)
