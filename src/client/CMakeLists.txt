file(GLOB_RECURSE CLIENT_SOURCE CONFIGURE_DEPENDS ./*.cc)
file(GLOB_RECURSE CORE_SOURCE CONFIGURE_DEPENDS ../core/*.cc)

add_library(
    zen-remote-client SHARED
    ${CLIENT_SOURCE}
    ${CORE_SOURCE}
)

target_include_directories(
    zen-remote-client
    PUBLIC ${public_inc}
    PRIVATE ${private_inc}
    PRIVATE ${boost_inc}
)

target_precompile_headers(zen-remote-client PRIVATE ../pch/pch.h)

target_compile_options(
    zen-remote-client
    PRIVATE -Wno-gnu-zero-variadic-macro-arguments
)

add_library(zen_remote::client ALIAS zen-remote-client)

install(TARGETS zen-remote-client)

configure_file(zen-remote-client.pc.in zen-remote-client.pc @ONLY) 

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/zen-remote-client.pc
    DESTINATION lib/pkgconfig
)
