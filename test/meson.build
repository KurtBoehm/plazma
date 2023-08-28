options_sub = subproject('options')
opts = []
args = options_sub.get_variable('all_args')

foreach name : ['alice.md', 'alice.md.xz']
  configure_file(copy: true, input: 'data/' + name, output: name)
endforeach

foreach name, info : {
  'AliceRead': [['alice-read.cpp'], []],
  'AliceWrite': [['alice-write.cpp'], []],
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
      dependencies: [plazma_dep] + deps,
    ),
  )
endforeach
