add_subdirectory(client)
add_subdirectory(server)

if(${ZEN_REMOTE_TEST_CLIENT})

  add_executable(
    test-client
    "test-client.cc"
  )
  
  target_link_libraries(
    test-client
    PRIVATE zen_remote::client
  )

endif()
