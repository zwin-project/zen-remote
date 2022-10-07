file(GLOB_RECURSE SERVER_SOURCE CONFIGURE_DEPENDS ./*.cc)
file(GLOB_RECURSE CORE_SOURCE CONFIGURE_DEPENDS ../core/*.cc)

add_library(
    zen_remote_server STATIC 
    ${SERVER_SOURCE}
    ${CORE_SOURCE}
)

target_include_directories(
    zen_remote_server
    PUBLIC ${public_inc}
    PRIVATE ${private_inc}
    PRIVATE ${boost_headers_content_SOURCE_DIR}/include
)

target_precompile_headers(zen_remote_server PRIVATE ../pch/pch.h)

target_compile_options(
    zen_remote_server
    PRIVATE -Wno-gnu-zero-variadic-macro-arguments
)

add_library(zen_remote::server ALIAS zen_remote_server)
