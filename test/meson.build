options_sub = subproject('options')
opts = options_sub.get_variable('all_opts')
args = options_sub.get_variable('all_args')

foreach name, info : {
  'Alice': [['alice.cpp'], []],
}
  sources = info[0]
  deps = info[1]
  test(
    name,
    executable(
      'Test' + name,
      sources,
      override_options: opts,
      cpp_args: args,
      dependencies: deps,
    ),
  )
endforeach
