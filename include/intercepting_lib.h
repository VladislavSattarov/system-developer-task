#pragma once
#include "log_entry.h"
#include "shared_buffer.h"
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <ctime>
#include <dlfcn.h>
#include <fcntl.h>
#include <iostream>
#include <stdarg.h>
#include <sys/time.h>
#include <sys/types.h>

static bool in_logging = false;

extern "C" {
int open(const char *filename, int flags, ...);
int close(int fd);
off_t lseek(int fd, off_t offset, int whence);
ssize_t read(int fd, void *buf, size_t count);
ssize_t write(int fd, const void *buf, size_t count);
void *malloc(size_t size);
void *realloc(void *ptr, size_t size);
void free(void *ptr);
}