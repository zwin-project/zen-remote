#include "server/job.h"

namespace zen::remote::server {

void
Job::Perform(bool cancel)
{
  perform_func_(cancel);
}

}  // namespace zen::remote::server
