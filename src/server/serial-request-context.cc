#include "server/serial-request-context.h"

#include "server/session.h"

namespace zen::remote::server {

SerialRequestContext::SerialRequestContext(Session *session)
{
  this->AddMetadata(kGrpcMetadataSerialKey,
      std::to_string(session->NewSerial(Session::kRequest)));
  this->AddMetadata(kGrpcMetadataSessionKey, std::to_string(session->id()));
}

}  // namespace zen::remote::server
