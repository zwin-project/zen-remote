#pragma once

#include "core/readers-writer-lock.h"

namespace zen::remote::client {

enum class ResourceContainerType {
  kLoopIntensive,
};

template <class T, ResourceContainerType type>
class ResourceContainer {
 public:
  void Add(std::shared_ptr<T> resource);

  std::shared_ptr<T> Get(uint64_t id);

  void ScheduleRemove(uint64_t id);

  void ForEach(std::function<void(const std::shared_ptr<T>&)> func);
};

template <class T>
class ResourceContainer<T, ResourceContainerType::kLoopIntensive> {
 public:
  void Add(std::shared_ptr<T> resource)
  {
    std::lock_guard<std::mutex> lock(new_resources_mtx_);
    new_resources_.push_back(std::move(resource));
  }

  std::shared_ptr<T> Get(uint64_t id)
  {
    /**
     * Search new_resources_ first, then resources_.
     * Elements are only moved from new_resources_ to resources_, so there is no
     * need to lock both at the same time.
     */
    {
      std::lock_guard<std::mutex> lock(new_resources_mtx_);
      auto result = std::find_if(new_resources_.begin(), new_resources_.end(),
          [id](std::shared_ptr<T>& resource) { return resource->id() == id; });
      if (result != new_resources_.end()) return *result;
    }

    {
      auto scope = resources_lock_.ReadLockGuard();
      auto result = std::find_if(resources_.begin(), resources_.end(),
          [id](std::shared_ptr<T>& resource) { return resource->id() == id; });
      if (result != new_resources_.end()) return *result;
    }

    return std::shared_ptr<T>();
  }

  void ScheduleRemove(uint64_t id)
  {
    std::lock_guard<std::mutex> lock(remove_set_mtx_);
    remove_set_.insert(id);
  }

  void ForEach(std::function<void(const std::shared_ptr<T>&)> func)
  {
    {  // apply new_resources_ and remove_set_
      auto scope = resources_lock_.WriteLockGuard();
      {
        std::lock_guard<std::mutex> lock(new_resources_mtx_);
        resources_.reserve(resources_.size() + new_resources_.size());
        std::move(new_resources_.begin(), new_resources_.end(),
            std::back_inserter(resources_));
        new_resources_.clear();
      }

      {
        std::lock_guard<std::mutex> lock(remove_set_mtx_);
        auto result = std::remove_if(resources_.begin(), resources_.end(),
            [this](std::shared_ptr<T>& resource) {
              return remove_set_.find(resource->id()) != remove_set_.end();
            });
        resources_.erase(result, resources_.end());
        remove_set_.clear();
      }
    }

    {
      auto scope = resources_lock_.ReadLockGuard();
      std::for_each(resources_.begin(), resources_.end(), func);
    }
  }

 private:
  std::vector<std::shared_ptr<T>> resources_;
  std::vector<std::shared_ptr<T>> new_resources_;
  std::set<uint64_t> remove_set_;

  /**
   * When you lock two or more, lock main_lock_ first.
   */
  ReadersWriterLock resources_lock_;
  std::mutex new_resources_mtx_;
  std::mutex remove_set_mtx_;
};

}  // namespace zen::remote::client
