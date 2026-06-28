# This file is part of https://github.com/KurtBoehm/plazma.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.

options_sub = subproject('options')
args = options_sub.get_variable('all_args')

foreach name : ['alice.md', 'alice.md.xz']
  configure_file(input: 'data/' + name, output: name, copy: true)
endforeach

foreach name, info : {
  'alice-read': [['alice-read.cpp'], []],
  'alice-write': [['alice-write.cpp'], []],
}
  sources = info[0]
  deps = info[1]
  test(
    name,
    executable(name, sources, dependencies: [plazma_dep] + deps, cpp_args: args),
  )
endforeach
