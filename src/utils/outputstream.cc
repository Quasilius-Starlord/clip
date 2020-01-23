/**
 * This file is part of the "clip" project
 *   Copyright (c) 2018 Paul Asmuth
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <cstdarg>
#include <fcntl.h>
#include <memory>
#include <stdarg.h>
#include <string>
#include <unistd.h>
#include "buffer.h"
#include "exception.h"
#include "outputstream.h"

namespace clip {

std::unique_ptr<OutputStream> OutputStream::getStdout() {
  auto stdout_stream = new FileOutputStream(1, false);
  return std::unique_ptr<OutputStream>(stdout_stream);
}

std::unique_ptr<OutputStream> OutputStream::getStderr() {
  auto stderr_stream = new FileOutputStream(2, false);
  return std::unique_ptr<OutputStream>(stderr_stream);
}

size_t OutputStream::write(const std::string& data) {
  return write(data.c_str(), data.size());
}

size_t OutputStream::write(const Buffer& buf) {
  return write((const char*) buf.data(), buf.size());
}

// FIXPAUL: variable size buffer
size_t OutputStream::printf(const char* format, ...) {
  char buf[8192];

  va_list args;
  va_start(args, format);
  int pos = vsnprintf(buf, sizeof(buf), format, args);
  va_end(args);

  if (pos < 0) {
    RAISE_ERRNO(kIOError, "vsnprintf() failed");
  }

  if (pos < sizeof(buf)) {
    write(buf, pos);
  } else {
    RAISE_ERRNO(kBufferOverflowError, "vsnprintf() failed: value too large");
  }

  return pos;
}

std::unique_ptr<FileOutputStream> FileOutputStream::openFile(
    const std::string& file_path,
    int flags /* = O_CREAT | O_TRUNC */,
    int permissions /* = 0666 */) {
  int fd = -1;
  auto fp = file_path.c_str();

  flags |= O_WRONLY;

  if ((flags & O_CREAT) == O_CREAT) {
    fd = open(fp, flags, permissions);
  } else {
    fd = open(fp, flags);
  }

  if (fd < 1) {
    RAISE_ERRNO(kIOError, "error opening file '%s'", fp);
  }

  return std::unique_ptr<FileOutputStream>(new FileOutputStream(fd, true));
}

FileOutputStream::FileOutputStream(
    int fd,
    bool close_on_destroy /* = false */) :
    fd_(fd),
    close_on_destroy_(close_on_destroy) {}

FileOutputStream::~FileOutputStream() {
  if (fd_ >= 0 && close_on_destroy_) {
    close(fd_);
  }
}

size_t FileOutputStream::write(const char* data, size_t size) {
  int bytes_written = -1;

  bytes_written = ::write(fd_, data, size);

  if (bytes_written < 0) {
    RAISE_ERRNO(kIOError, "write() failed");
  }

  return bytes_written;
}

size_t FileOutputStream::printf(const char* format, ...) {
  va_list args;
  va_start(args, format);
  int pos = vdprintf(fd_, format, args);
  va_end(args);

  if (pos < 0) {
    RAISE_ERRNO(kIOError, "vdprintf() failed");
  }

  return pos;
}

std::unique_ptr<StringOutputStream> StringOutputStream::fromString(
    std::string* string) {
  return std::unique_ptr<StringOutputStream>(new StringOutputStream(string));
}

StringOutputStream::StringOutputStream(std::string* string) : str_(string) {}

size_t StringOutputStream::write(const char* data, size_t size) {
  *str_ += std::string(data, size);
  return size;
}

std::unique_ptr<BufferOutputStream> BufferOutputStream::fromBuffer(
    Buffer* buf) {
  return std::unique_ptr<BufferOutputStream>(new BufferOutputStream(buf));
}

BufferOutputStream::BufferOutputStream(Buffer* buf) : buf_(buf) {}

size_t BufferOutputStream::write(const char* data, size_t size) {
  buf_->append(data, size);
  return size;
}

} // fnord

