#pragma once

#include "core/common.h"

namespace zen::remote {

/**
 * @brief writer-preferring
 */
class ReadersWriterLock {
  class ReadScope {
   public:
    DISABLE_MOVE_AND_COPY(ReadScope);
    ReadScope() = delete;
    ReadScope(ReadersWriterLock* lock);

    ~ReadScope();

   private:
    ReadersWriterLock* lock_;
  };

  class WriteScope {
   public:
    DISABLE_MOVE_AND_COPY(WriteScope);
    WriteScope() = delete;
    WriteScope(ReadersWriterLock* lock);

    ~WriteScope();

   private:
    ReadersWriterLock* lock_;
  };

 public:
  DISABLE_MOVE_AND_COPY(ReadersWriterLock);
  ReadersWriterLock() = default;

  std::unique_ptr<ReadScope> ReadLockGuard();

  std::unique_ptr<WriteScope> WriteLockGuard();

 private:
  void BeginRead();
  void EndRead();
  void BeginWrite();
  void EndWrite();

  uint32_t active_reader_count_ = 0;
  uint32_t waiting_writer_count_ = 0;
  bool writer_active_ = false;
  std::condition_variable cond_;
  std::mutex mtx_;
};

}  // namespace zen::remote
