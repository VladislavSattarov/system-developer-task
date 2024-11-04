#pragma once

#include <ctime>
#include <cstddef>
#include <sys/types.h>

extern "C" {
    int open(const char *filename, int flags, ...);
    int close(int fd);
    off_t lseek(int fd, off_t offset, int whence);
    ssize_t read(int fd, void *buf, size_t count);
    ssize_t write(int fd, const void *buf, size_t count);
    void* malloc(size_t size);
    void* realloc(void* ptr, size_t size);
    void free(void* ptr);
}