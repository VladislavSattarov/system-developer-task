#pragma once

enum class LogType : uint8_t {
	OPEN,
	CLOSE,
	LSEEK,
	READ,
	WRITE,
	MALLOC,
	REALLOC,
	FREE
};

struct LogEntry {
	uint64_t timestamp;
	LogType type;
	int file_descriptor;
	const char *filename;
	int flags;
	int whence;
	size_t size;
	off_t offset;
	const void *pointer;
	int fd_rc;
	off_t resulted_offset;
	ssize_t bytes_rw;
	void *new_mem_pointer;
};