core_srcs = [
  'core/connection/peer.cc',
  'core/logger.cc',
]

client_srcs = [
  'client/remote.cc'
] + core_srcs

server_srcs = [
  'server/remote.cc',
] + core_srcs

zen_remote_client_lib = static_library(
  'zen-remote-client',
  client_srcs,
  include_directories: [ public_inc, private_inc, boost_inc ],
  cpp_pch: 'pch/pch.h',
  install: false,
)

zen_remote_client_dep = declare_dependency(
  link_with: zen_remote_client_lib,
  include_directories: public_inc,
)

zen_remote_server_lib = static_library(
  'zen-remote-server',
  server_srcs,
  include_directories: [ public_inc, private_inc, boost_inc ],
  cpp_pch: 'pch/pch.h',
  install: false,
)

zen_remote_server_dep = declare_dependency(
  link_with: zen_remote_server_lib,
  include_directories: public_inc,
)

executable(
  'test-client',
  [ 'test-client.cc' ],
  install: false,
  dependencies: zen_remote_client_dep,
)
