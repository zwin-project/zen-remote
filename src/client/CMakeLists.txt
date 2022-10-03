file(GLOB_RECURSE CLIENT_SOURCE CONFIGURE_DEPENDS ./*.cc)
file(GLOB_RECURSE CORE_SOURCE CONFIGURE_DEPENDS ../core/*.cc)

add_library(
    zen_remote_display_system_client STATIC 
    ${CLIENT_SOURCE}
    ${CORE_SOURCE}
)

target_include_directories(
    zen_remote_display_system_client
    PUBLIC ${public_inc}
    PRIVATE ${private_inc}
    PRIVATE ${boost_headers_content_SOURCE_DIR}/include
)

target_precompile_headers(zen_remote_display_system_client PRIVATE ../pch.h)

target_compile_options(
    zen_remote_display_system_client
    PRIVATE -Wno-gnu-zero-variadic-macro-arguments
)

add_library(zen_remote_display_system::client ALIAS zen_remote_display_system_client)
