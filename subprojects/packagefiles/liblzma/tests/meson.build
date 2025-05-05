# This file is part of https://github.com/KurtBoehm/tlaxcaltin.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.

liblzma_tests = [
  # 'test_bcj_exact_size',
  'test_block_header',
  'test_check',
  'test_filter_flags',
  'test_index',
  'test_stream_flags',
]

tinc = [include_directories('../src/common', '../src/liblzma'), confinc]

foreach t : liblzma_tests
  test(
    t,
    executable(
      t,
      '@0@.c'.format(t),
      c_args: '-DHAVE_CONFIG_H',
      include_directories: tinc,
      dependencies: lzma_dep,
    ),
  )
endforeach
