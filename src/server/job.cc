#include "server/job.h"

namespace zen::remote::server {

Job::Job(std::function<void(bool cancel)> perform_func)
    : perform_func_(perform_func)
{
}

void
Job::Perform(bool cancel)
{
  perform_func_(cancel);
}

}  // namespace zen::remote::server
