#include "log_entry.h"
#include "shared_buffer.h"
#include <chrono>
#include <csignal>
#include <cstring>
#include <fstream>
#include <iostream>
#include <thread>

using namespace boost::interprocess;

struct DemonConfig {
	std::string log_file;
	int poll_interval_ms;
};

using ShmemAllocator = boost::interprocess::allocator<
	LogEntry, boost::interprocess::managed_shared_memory::segment_manager>;
using Queue = boost::interprocess::deque<LogEntry, ShmemAllocator>;

void LogToFile(const DemonConfig &config, const LogEntry &entry)
{
	std::ofstream log_file(config.log_file, std::ios_base::app);
	if (!log_file.is_open()) {
		std::cerr << "Error opening log file: " << config.log_file
			  << std::endl;
		return;
	}

	log_file << "Timestamp: " << entry.timestamp << ", Function: ";
	switch (entry.type) {
	case LogType::OPEN:
		log_file << "OPEN, Filename: " << entry.filename
			 << ", Flags: " << entry.flags
			 << ", FD: " << entry.file_descriptor;
		break;
	case LogType::CLOSE:
		log_file << "CLOSE, FD: " << entry.file_descriptor
			 << ", Return Code: " << entry.fd_rc;
		break;
	case LogType::LSEEK:
		log_file << "LSEEK, FD: " << entry.file_descriptor
			 << ", Requested offset: " << entry.offset
			 << ", Resulted offset: " << entry.resulted_offset;
		break;
	case LogType::READ:
		log_file << "READ, FD: " << entry.file_descriptor
			 << ", Buffer pointer: " << entry.pointer
			 << ", Count: " << entry.size
			 << ", Bytes read: " << entry.bytes_rw;
		break;
	case LogType::WRITE:
		log_file << "WRITE, FD: " << entry.file_descriptor
			 << ", Buffer pointer: " << entry.pointer
			 << ", Count: " << entry.size
			 << ", Bytes written: " << entry.bytes_rw;
		break;
	case LogType::MALLOC:
		log_file << "MALLOC, Bytes requested: " << entry.size
			 << ", New mem pointer: " << entry.new_mem_pointer;
		break;
	case LogType::REALLOC:
		log_file << "MALLOC, Bytes requested: " << entry.size
			 << ", Current mem pointer: " << entry.pointer
			 << ", New mem pointer: " << entry.new_mem_pointer;
		break;
	case LogType::FREE:
		log_file << "FREE, Mem pointer: " << entry.pointer;
		break;
	default:
		log_file << "UNKNOWN";
		break;
	}
	log_file << std::endl;
	log_file.close();
}

void MemoryDemon(SharedBuffer<LogEntry> &buffer, const DemonConfig &config)
{
	LogEntry entry;
	while (true) {
		if (buffer.GetData(entry)) {
			if (entry.type == LogType::MALLOC ||
			    entry.type == LogType::REALLOC ||
			    entry.type == LogType::FREE) {
				LogToFile(config, entry);
			}
		}
		std::this_thread::sleep_for(
			std::chrono::milliseconds(config.poll_interval_ms));
	}
}

void FileIODemon(SharedBuffer<LogEntry> &buffer, const DemonConfig &config)
{
	LogEntry entry;
	while (true) {
		if (buffer.GetData(entry)) {
			if (entry.type == LogType::OPEN ||
			    entry.type == LogType::CLOSE ||
			    entry.type == LogType::LSEEK ||
			    entry.type == LogType::READ ||
			    entry.type == LogType::WRITE) {
				LogToFile(config, entry);
			}
		}
		std::this_thread::sleep_for(
			std::chrono::milliseconds(config.poll_interval_ms));
	}
}

int main(int argc, char *argv[])
{
	if (argc < 4) {
		std::cerr << "Usage: " << argv[0]
			  << " [--file | --mem] <poll_interval_ms> <log_file>"
			  << std::endl;
		return 1;
	}
	std::string demon_type = argv[1];
	int poll_interval_ms = std::stoi(argv[2]);
	std::string log_file = argv[3];

	DemonConfig config = { log_file, poll_interval_ms };

	managed_shared_memory segment(open_only, "shared_log_buffer");
	Queue *queue = segment.find<Queue>("Queue").first;
	queue->front();
	SharedBuffer<LogEntry> shared_buffer("shared_log_buffer", false);

	if (demon_type == "--file") {
		FileIODemon(shared_buffer, config);
	} else if (demon_type == "--mem") {
		MemoryDemon(shared_buffer, config);
	} else {
		std::cerr << "Unknown demon type: " << demon_type << std::endl;
		return 1;
	}

	return 0;
}