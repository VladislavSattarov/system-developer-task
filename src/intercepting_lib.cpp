#include "intercepting_lib.h"

static SharedBuffer<LogEntry> shared_buffer("shared_log_buffer", true);

static inline uint64_t GetCurrentTime()
{
	timeval time;
	gettimeofday(&time, nullptr);
	return time.tv_sec * 1000 + time.tv_usec / 1000;
}

int open(const char *filename, int flags, ...)
{
	static int (*original_open)(const char *, int, ...) = nullptr;
	if (!original_open) {
		original_open = (int (*)(const char *, int,
					 ...))dlsym(RTLD_NEXT, "open");
	}

	// Логи
	uint64_t tm_stamp = GetCurrentTime();

	int fd;
	va_list args;
	va_start(args, flags);
	if (flags & O_CREAT) {
		mode_t mode = va_arg(args, mode_t);
		fd = original_open(filename, flags, mode);
	} else {
		fd = original_open(filename, flags);
	}
	va_end(args);

	LogEntry log;
	log.timestamp = tm_stamp;
	log.filename = filename;
	log.type = LogType::OPEN;
	log.fd_rc = fd;
	log.file_descriptor = fd;
	log.flags = flags;

	shared_buffer.AddData(log);
	return fd;
}

int close(int fd)
{
	static int (*original_close)(int) = nullptr;
	if (!original_close) {
		original_close = (int (*)(int))dlsym(RTLD_NEXT, "close");
	}

	uint64_t tm_stamp = GetCurrentTime();

	int return_code = original_close(fd);

	LogEntry log;
	log.timestamp = tm_stamp;
	log.type = LogType::CLOSE;
	log.fd_rc = return_code;
	log.file_descriptor = fd;

	shared_buffer.AddData(log);
	return return_code;
}

off_t lseek(int fd, off_t offset, int whence)
{
	static off_t (*original_lseek)(int, off_t, int) = nullptr;
	if (!original_lseek) {
		original_lseek =
			(off_t(*)(int, off_t, int))dlsym(RTLD_NEXT, "lseek");
	}

	uint64_t tm_stamp = GetCurrentTime();

	off_t return_offset = original_lseek(fd, offset, whence);

	LogEntry log;
	log.timestamp = tm_stamp;
	log.type = LogType::LSEEK;
	log.offset = offset;
	log.resulted_offset = return_offset;
	log.whence = whence;
	log.file_descriptor = fd;

	shared_buffer.AddData(log);
	return return_offset;
}

ssize_t read(int fd, void *buf, size_t count)
{
	static ssize_t (*original_read)(int, void *, size_t) = nullptr;
	if (!original_read) {
		original_read = (ssize_t(*)(int, void *,
					    size_t))dlsym(RTLD_NEXT, "read");
	}

	uint64_t tm_stamp = GetCurrentTime();

	ssize_t bytes_read = original_read(fd, buf, count);

	LogEntry log;
	log.timestamp = tm_stamp;
	log.type = LogType::READ;
	log.pointer = buf;
	log.file_descriptor = fd;
	log.size = count;
	log.bytes_rw = bytes_read;

	shared_buffer.AddData(log);
	return bytes_read;
}

ssize_t write(int fd, const void *buf, size_t count)
{
	static ssize_t (*original_write)(int, const void *, size_t) = nullptr;
	if (!original_write) {
		original_write = (ssize_t(*)(int, const void *,
					     size_t))dlsym(RTLD_NEXT, "write");
	}

	uint64_t tm_stamp = GetCurrentTime();

	ssize_t bytes_written = original_write(fd, buf, count);

	LogEntry log;
	log.timestamp = tm_stamp;
	log.type = LogType::WRITE;
	log.pointer = buf;
	log.file_descriptor = fd;
	log.size = count;
	log.bytes_rw = bytes_written;

	shared_buffer.AddData(log);
	return bytes_written;
}

void *malloc(size_t size)
{
	static void *(*original_malloc)(size_t) = nullptr;
	if (!original_malloc) {
		original_malloc = (void *(*)(size_t))dlsym(RTLD_NEXT, "malloc");
	}

	uint64_t tm_stamp = GetCurrentTime();

	void *new_mem_pointer = original_malloc(size);
	if (!in_logging) {
		in_logging = true;
		LogEntry log;
		log.timestamp = tm_stamp;
		log.type = LogType::MALLOC;
		log.pointer = new_mem_pointer;
		log.size = size;
		log.new_mem_pointer = new_mem_pointer;

		shared_buffer.AddData(log);
	}
	in_logging = false;
	return new_mem_pointer;
}

void *realloc(void *ptr, size_t size)
{
	static void *(*original_realloc)(void *, size_t) = nullptr;
	if (!original_realloc) {
		original_realloc =
			(void *(*)(void *, size_t))dlsym(RTLD_NEXT, "realloc");
	}

	uint64_t tm_stamp = GetCurrentTime();

	void *new_mem_pointer = original_realloc(ptr, size);

	LogEntry log;
	log.timestamp = tm_stamp;
	log.type = LogType::REALLOC;
	log.pointer = ptr;
	log.size = size;
	log.new_mem_pointer = new_mem_pointer;

	shared_buffer.AddData(log);
	return new_mem_pointer;
}

void free(void *ptr)
{
	static void (*original_free)(void *) = nullptr;
	if (!original_free) {
		original_free = (void (*)(void *))dlsym(RTLD_NEXT, "free");
	}

	uint64_t tm_stamp = GetCurrentTime();

	original_free(ptr);
	if (!in_logging) {
		in_logging = true;
		LogEntry log;
		log.timestamp = tm_stamp;
		log.pointer = ptr;

		shared_buffer.AddData(log);
	}
	in_logging = false;
	return;
}