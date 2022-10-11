file(GLOB_RECURSE SERVER_SOURCE CONFIGURE_DEPENDS ./*.cc)
file(GLOB_RECURSE CORE_SOURCE CONFIGURE_DEPENDS ../core/*.cc)

add_library(
  zen-remote-server SHARED
  ${SERVER_SOURCE}
  ${CORE_SOURCE}
)

target_include_directories(
  zen-remote-server
  PUBLIC ${zen_remote_public_headers}
  PRIVATE ${zen_remote_private_headers}
  PRIVATE ${zen_remote_boost_headers}
)

target_link_libraries(
  zen-remote-server
  PRIVATE zen_remote_grpc_proto
)

target_precompile_headers(zen-remote-server PRIVATE ../pch/pch.h)

target_compile_options(
  zen-remote-server
  PRIVATE
    -Wall -Winvalid-pch -Wextra -Wpedantic -Werror
    -Wno-gnu-zero-variadic-macro-arguments
)

add_library(zen_remote::server ALIAS zen-remote-server)

install(TARGETS zen-remote-server)

configure_file(zen-remote-server.pc.in zen-remote-server.pc @ONLY) 

install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/zen-remote-server.pc
  DESTINATION lib/pkgconfig
)
