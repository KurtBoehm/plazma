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
    override_options: opts,
    cpp_args: args,
    dependencies: [plazma_dep] + deps,
  )
endforeach
