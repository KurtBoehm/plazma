# This file is part of https://github.com/KurtBoehm/tlaxcaltin.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.

project(
  'fmt',
  'cpp',
  version: '12.1.0',
  meson_version: '>=0.50.0',
  license: 'MIT WITH fmt-exception',
)

fmt_private_cpp_args = []
fmt_interface_cpp_args = []

if get_option('default_library') == 'shared'
  fmt_private_cpp_args += ['-DFMT_LIB_EXPORT']
  fmt_interface_cpp_args += ['-DFMT_SHARED']
endif

header_only = get_option('header-only')

if header_only
  fmt_header_only_dep = declare_dependency(
    include_directories: 'include',
    compile_args: ['-DFMT_HEADER_ONLY'],
  )
else
  fmt_lib = library(
    'fmt',
    'src/format.cc',
    'src/os.cc',
    include_directories: 'include',
    override_options: ['cpp_std=c++14'],
    cpp_args: fmt_private_cpp_args,
    build_by_default: true,
    install: not meson.is_subproject(),
  )

  fmt_dep = declare_dependency(
    include_directories: 'include',
    link_with: fmt_lib,
    compile_args: fmt_interface_cpp_args,
  )

  if not meson.is_subproject()
    install_subdir(
      'include',
      strip_directory: true,
      install_dir: get_option('includedir'),
    )

    pc = import('pkgconfig')
    pc.generate(
      fmt_lib,
      name: 'fmt',
      filebase: 'fmt',
    )
  endif
endif
