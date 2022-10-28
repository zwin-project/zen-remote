if (${ZEN_REMOTE_CLIENT})
  add_subdirectory(client)
endif()

if (${ZEN_REMOTE_SERVER})
  add_subdirectory(server)
endif()
