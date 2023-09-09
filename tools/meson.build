options_sub = subproject('options')
opts = []
args = (
  options_sub.get_variable('all_args') + options_sub.get_variable(
    'optimization_args',
  )
)

argparse_dep = dependency('argparse')

foreach name, info : {
  'Info': [['info.cpp'], []],
}
  sources = info[0]
  deps = info[1]
  test(
    name,
    executable(
      name,
      sources,
      override_options: opts,
      cpp_args: args,
      dependencies: [plazma_dep, argparse_dep] + deps,
    ),
  )
endforeach
