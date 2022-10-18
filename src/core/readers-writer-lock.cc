#include "core/readers-writer-lock.h"

namespace zen::remote {

ReadersWriterLock::ReadScope::~ReadScope() { lock_->EndRead(); }

ReadersWriterLock::WriteScope::~WriteScope() { lock_->EndWrite(); }

std::unique_ptr<ReadersWriterLock::ReadScope>
ReadersWriterLock::ReadLockGuard()
{
  BeginRead();
  return std::make_unique<ReadersWriterLock::ReadScope>(this);
}

std::unique_ptr<ReadersWriterLock::WriteScope>
ReadersWriterLock::WriteLockGuard()
{
  BeginWrite();
  return std::make_unique<ReadersWriterLock::WriteScope>(this);
}

void
ReadersWriterLock::BeginRead()
{
  std::unique_lock<std::mutex> lock(mtx_);
  cond_.wait(
      lock, [this] { return waiting_writer_count_ == 0 && !writer_active_; });
  active_reader_count_++;
}

void
ReadersWriterLock::EndRead()
{
  std::lock_guard<std::mutex> lock(mtx_);
  active_reader_count_--;
  if (active_reader_count_ == 0) cond_.notify_all();
}

void
ReadersWriterLock::BeginWrite()
{
  std::unique_lock<std::mutex> lock(mtx_);
  waiting_writer_count_++;
  cond_.wait(lock,
      [this] { return active_reader_count_ == 0 && writer_active_ == false; });
  waiting_writer_count_--;
  writer_active_ = true;
}

void
ReadersWriterLock::EndWrite()
{
  std::lock_guard<std::mutex> lock(mtx_);
  writer_active_ = false;
  cond_.notify_all();
}

}  // namespace zen::remote
