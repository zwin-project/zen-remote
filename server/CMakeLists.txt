file(GLOB_RECURSE SERVER_SOURCE CONFIGURE_DEPENDS ./*.cc)
file(GLOB_RECURSE CORE_SOURCE CONFIGURE_DEPENDS ../core/*.cc)

add_library(
    zen_remote_display_system_server STATIC 
    ${SERVER_SOURCE}
    ${CORE_SOURCE}
)

target_include_directories(
    zen_remote_display_system_server
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include
    PRIVATE ../core
)

target_compile_options(
    zen_remote_display_system_server
    PRIVATE -Wno-gnu-zero-variadic-macro-arguments
)

add_library(zen_remote_display_system::server ALIAS zen_remote_display_system_server)
