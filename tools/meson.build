# This file is part of https://github.com/KurtBoehm/plazma.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.

options_sub = subproject('options')
opts = []
args = options_sub.get_variable('all_args') + options_sub.get_variable('optimization_args')

foreach name, deps : {
  'compress': [],
  'info': [],
}
  executable(
    name,
    [f'@name@.cpp'],
    dependencies: [plazma_dep] + deps,
    override_options: opts,
    cpp_args: args,
  )
endforeach
