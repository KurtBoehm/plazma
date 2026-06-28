# This file is part of https://github.com/KurtBoehm/tlaxcaltin.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.

project('pcg-cpp', 'cpp', license: 'Apache 2.0')

install_headers('include/pcg_extras.hpp')
install_headers('include/pcg_random.hpp')
install_headers('include/pcg_uint128.hpp')

import('pkgconfig').generate(
  name: 'pcg-cpp',
  version: meson.project_version(),
  description: 'PCG Random Number Generation, C++ Edition',
)

pcg_cpp_dep = declare_dependency(
  include_directories: include_directories('include'),
)
