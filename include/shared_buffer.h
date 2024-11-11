#pragma once
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/deque.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <iostream>
#include <stdexcept>
#include <string>

#define BUFFER_SIZE 1024

template <class T> class SharedBuffer {
    public:
	SharedBuffer(const std::string &name = "shared_log_buffer", bool create = true,
		     size_t buffer_size = BUFFER_SIZE * sizeof(T))
		: buffer_name_(name)
		, buffer_size_(buffer_size)
		, create(create)
	{
		mutex_name_ = buffer_name_ + "_mutex";

		if (create) {
			boost::interprocess::shared_memory_object::remove(
				buffer_name_.c_str());

			segment_ = boost::interprocess::managed_shared_memory(
				boost::interprocess::create_only,
				buffer_name_.c_str(), buffer_size_);

			const ShmemAllocator allocator(
				segment_.get_segment_manager());
			queue_ = segment_.construct<Queue>("Queue")(allocator);
		} else {
			segment_ = boost::interprocess::managed_shared_memory(
				boost::interprocess::open_only,
				buffer_name_.c_str());

			queue_ = segment_.find<Queue>("Queue").first;
			if (!queue_) {
				throw std::runtime_error(
					"Failed to find shared queue in shared memory");
			}
		}
	}

	~SharedBuffer()
	{
		if (create) {
			boost::interprocess::shared_memory_object::remove(
				buffer_name_.c_str());
		}
	}

	bool AddData(const T &data)
	{
		if (queue_ == nullptr) {
			return false;
		}
		if (queue_->size() >= buffer_size_) {
			queue_->pop_front();
			queue_->push_back(data);
			return true;
		}
		queue_->push_back(data);
		return true;
	}

	bool GetData(T &data)
	{
		if (queue_->empty()) {
			return false;
		}

		data = queue_->front();
		queue_->pop_front();
		return true;
	}

    private:
	using ShmemAllocator = boost::interprocess::allocator<
		T, boost::interprocess::managed_shared_memory::segment_manager>;
	using Queue = boost::interprocess::deque<T, ShmemAllocator>;

	std::string buffer_name_;
	std::string mutex_name_;
	size_t buffer_size_;
	bool create;

	Queue *queue_;
	boost::interprocess::managed_shared_memory segment_;
};
