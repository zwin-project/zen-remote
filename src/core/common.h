#pragma once

namespace zen::remote {

#define DISABLE_MOVE_AND_COPY(Class)        \
  Class(const Class &) = delete;            \
  Class(Class &&) = delete;                 \
  Class &operator=(const Class &) = delete; \
  Class &operator=(Class &&) = delete

inline constexpr uint16_t kDiscoverPort = 9983;  // listen in server
inline constexpr uint16_t kGrpcPort = 50051;     // listen in client

}  // namespace zen::remote
