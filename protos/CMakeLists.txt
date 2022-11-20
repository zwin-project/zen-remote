if (NOT DEFINED ZEN_REMOTE_GRPC_SYSROOT)
  message(FATAL_ERROR "ZEN_REMOTE_GRPC_SYSROOT is not set")
endif()

list(APPEND CMAKE_FIND_ROOT_PATH ${ZEN_REMOTE_GRPC_SYSROOT})

find_package(protobuf 3.21.6.0 EXACT REQUIRED CONFIG)
find_package(gRPC 1.49.1 EXACT REQUIRED CONFIG)

if (NOT DEFINED ZEN_REMOTE_PROTOC_EXECUTABLE)
  find_program(ZEN_REMOTE_PROTOC_EXECUTABLE protoc REQUIRED)
endif()

if (NOT DEFINED ZEN_REMOTE_GRPC_CPP_PLUGIN_EXECUTABLE)
  find_program(ZEN_REMOTE_GRPC_CPP_PLUGIN_EXECUTABLE grpc_cpp_plugin REQUIRED)
endif()

message(STATUS "protoc: ${ZEN_REMOTE_PROTOC_EXECUTABLE}")
message(STATUS "grpc_cpp_plugin: ${ZEN_REMOTE_GRPC_CPP_PLUGIN_EXECUTABLE}")

add_library(zen_remote_grpc_proto STATIC)

set(protos
  "common"
  "gl-base-technique"
  "gl-buffer"
  "gl-program"
  "gl-shader"
  "gl-texture"
  "gl-vertex-array"
  "rendering-unit"
  "session"
  "virtual-object"
)

foreach(proto IN LISTS protos)

  set(proto_cc "${CMAKE_CURRENT_BINARY_DIR}/${proto}.pb.cc")
  set(proto_h  "${CMAKE_CURRENT_BINARY_DIR}/${proto}.pb.h")
  set(grpc_cc  "${CMAKE_CURRENT_BINARY_DIR}/${proto}.grpc.pb.cc")
  set(grpc_h   "${CMAKE_CURRENT_BINARY_DIR}/${proto}.grpc.pb.h")

  add_custom_command(
    OUTPUT 
      "${proto_cc}"
      "${proto_h}"
      "${grpc_cc}"
      "${grpc_h}"
    COMMAND ${ZEN_REMOTE_PROTOC_EXECUTABLE}
    ARGS
      --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
      --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
      -I "${CMAKE_CURRENT_SOURCE_DIR}"
      --plugin=protoc-gen-grpc="${ZEN_REMOTE_GRPC_CPP_PLUGIN_EXECUTABLE}"
      "${CMAKE_CURRENT_SOURCE_DIR}/${proto}.proto"
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${proto}.proto"
  )

  target_sources(
    zen_remote_grpc_proto

    PRIVATE ${proto_cc}
    PRIVATE ${proto_h}
    PRIVATE ${grpc_cc}  
    PRIVATE ${grpc_h} 
  )

endforeach()

target_link_libraries(
  zen_remote_grpc_proto
  
  PUBLIC
    gRPC::grpc++
    protobuf::libprotobuf
)

set_target_properties(
  zen_remote_grpc_proto PROPERTIES
  COMPILE_FLAGS -fPIC
)

target_include_directories(
  zen_remote_grpc_proto

  PUBLIC ${ZEN_REMOTE_GRPC_SYSROOT}/include
  INTERFACE ${CMAKE_CURRENT_BINARY_DIR}
)
