#include "server/serial-request-context.h"

#include "server/channel.h"

namespace zen::remote::server {

SerialRequestContext::SerialRequestContext(
    const std::shared_ptr<Channel> &channel)
{
  this->AddMetadata(kGrpcMetadataSerialKey,
      std::to_string(channel->NewSerial(Channel::kRequest)));
  this->AddMetadata(
      kGrpcMetadataSessionKey, std::to_string(channel->session_id()));
  this->AddMetadata(kGrpcMetadataChannelKey, std::to_string(channel->id()));
}

}  // namespace zen::remote::server
