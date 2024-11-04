#include "intercepting_lib.h"
#include <dlfcn.h>
#include <iostream>
#include <sys/time.h>
#include <cstring>
#include <cstdint>
#include <stdarg.h>
#include <fcntl.h>


static inline uint64_t GetCurrentTime() {
    timeval time;
    gettimeofday(&time, nullptr);
    return time.tv_sec * 1000 + time.tv_usec / 1000;
}

int open(const char *filename, int flags, ...) {
    static int (*original_open)(const char*, int, ...) = nullptr;
    if (!original_open) {
        original_open = (int (*)(const char*, int, ...))dlsym(RTLD_NEXT, "open");
    }

    // Логи
    std::cout << "open() called at: " << GetCurrentTime() << ", filename: " << filename << "\n";

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

    std::cout << "open() called with flags: " << flags << "\n";
    std::cout << "open() returned: " << fd << "\n";

    return fd;
}

int close(int fd){
    static int (*original_close)(int) = nullptr;
    if (!original_close) {
        original_close = (int (*)(int))dlsym(RTLD_NEXT, "close");
    }

    std::cout << "close() called at: " << GetCurrentTime() << ", file_descriptor: " << fd << "\n";
    int return_code = original_close(fd);
    std::cout << "close() returned: " << return_code << "\n";
    return return_code;
}

off_t lseek(int fd, off_t offset, int whence){
    static off_t (*original_lseek)(int, off_t, int) = nullptr;
    if (!original_lseek) {
        original_lseek = (off_t (*)(int, off_t, int))dlsym(RTLD_NEXT, "lseek");
    }

    std::cout << "lseek() called at: " << GetCurrentTime() << ", file_descriptor: " << fd << ", offset: " << offset << ", whence: " << whence << "\n";
    int return_offset = original_lseek(fd, offset, whence);
    std::cout << "lseek() returned: " << return_offset << "\n";
    return return_offset;
}

ssize_t read(int fd, void *buf, size_t count) {
    static ssize_t (*original_read)(int, void *, size_t) = nullptr;
    if (!original_read) {
        original_read = (ssize_t (*)(int, void *, size_t))dlsym(RTLD_NEXT, "read");
    }

    std::cout << "read() called at: " << GetCurrentTime() << ", file_descriptor: " << fd << ", buffer_pointer: " << buf << ", count: " << count << "\n";
    int bytes_read = original_read(fd, buf, count);
    std::cout << "read() returned: " << bytes_read << "\n";
    return bytes_read;
}

ssize_t write(int fd, const void *buf, size_t count) {
    static ssize_t (*original_write)(int, const void *, size_t) = nullptr;
    if (!original_write) {
        original_write = (ssize_t (*)(int, const void *, size_t))dlsym(RTLD_NEXT, "write");
    }

    std::cout << "write() called at: " << GetCurrentTime() << ", file_descriptor: " << fd << ", buffer_pointer: " << buf << ", count: " << count << "\n";
    int bytes_written = original_write(fd, buf, count);
    std::cout << "write() returned: " << bytes_written << "\n";
    return bytes_written;
}

void* malloc(size_t size) {
    static void* (*original_malloc)(size_t) = nullptr;
    if (!original_malloc) {
        original_malloc = (void* (*)(size_t))dlsym(RTLD_NEXT, "malloc");
    }

    std::cout << "malloc() called at: " << GetCurrentTime() << ", bytes_requested: " << size << "\n";
    void* new_mem_pointer = original_malloc(size);
    std::cout << "malloc() returned: " << new_mem_pointer << "\n";
    return new_mem_pointer;
}

void* realloc(void* ptr, size_t size) {
    static void* (*original_realloc)(void*, size_t) = nullptr;
    if (!original_realloc) {
        original_realloc = (void* (*)(void*, size_t))dlsym(RTLD_NEXT, "realloc");
    }

    std::cout << "realloc() called at: " << GetCurrentTime() << ", bytes_requested: " << size << "current_mem_pointer" << ptr << "\n";
    void* new_mem_pointer = original_realloc(ptr, size);
    std::cout << "realloc() returned: " << new_mem_pointer << "\n";
    return new_mem_pointer;
}

void free(void* ptr){
    static void (*original_free)(void*) = nullptr;
    if (!original_free) {
        original_free = (void (*)(void*))dlsym(RTLD_NEXT, "free");
    }

    std::cout << "free() called at: " << GetCurrentTime() << ", mem_pointer: " << ptr << "\n";
    return original_free(ptr);
}