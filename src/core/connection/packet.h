#pragma once

namespace zen::remote::connection {

#define DISCOVER_PACKET_PREAMBLE "55d65470abe34d2ca21c21e4eb1033d5"  // UUID
#define DISCOVER_PACKET_VERSION 1

struct DiscoverPacket {
  DiscoverPacket()
  {
    memcpy(preamble, DISCOVER_PACKET_PREAMBLE, sizeof(preamble));
  }

  uint8_t preamble[32];
  uint16_t version = DISCOVER_PACKET_VERSION;
} __attribute__((__packed__));

}  // namespace zen::remote::connection
