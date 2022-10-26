#include "server/serial-request-context.h"

#include "server/remote.h"

namespace zen::remote::server {

SerialRequestContext::SerialRequestContext(
    const std::shared_ptr<Remote> &remote)
{
  this->AddMetadata(kGrpcMetadataSerialKey,
      std::to_string(remote->NewSerial(Remote::kRequest)));
}

}  // namespace zen::remote::server
