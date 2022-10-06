add_subdirectory(client)
add_subdirectory(server)

add_executable(
  test-client
  "test-client.cc"
)

target_link_libraries(
  test-client
  PRIVATE zen_remote_display_system_client
)
