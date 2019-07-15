/**
 * This file is part of the "fviz" project
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
#include <dirent.h>
#include <string.h>
#include <sys/fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sstream>
#include <fstream>
#include "buffer.h"
#include "exception.h"
#include "stringutil.h"
#include "fileutil.h"
#include "file.h"

namespace fviz {

void FileUtil::mkdir(const std::string& dirname) {
  if (::mkdir(dirname.c_str(), S_IRWXU) != 0) {
    RAISE_ERRNO(kIOError, "mkdir('%s') failed", dirname.c_str());
  }
}

bool FileUtil::exists(const std::string& filename) {
  struct stat fstat;

  if (stat(filename.c_str(), &fstat) < 0) {
    if (errno == ENOENT) {
      return false;
    }

    RAISE_ERRNO(kIOError, "fstat('%s') failed", filename.c_str());
  }

  return true;
}

bool FileUtil::isDirectory(const std::string& filename) {
  struct stat fstat;

  if (stat(filename.c_str(), &fstat) < 0) {
    RAISE_ERRNO(kIOError, "fstat('%s') failed", filename.c_str());
  }

  return S_ISDIR(fstat.st_mode);
}

size_t FileUtil::size(const std::string& filename) {
  struct stat fstat;

  if (stat(filename.c_str(), &fstat) < 0) {
    RAISE_ERRNO(kIOError, "fstat('%s') failed", filename.c_str());
  }

  return fstat.st_size;
}

/* The mkdir_p method was adapted from bash 4.1 */
void FileUtil::mkdir_p(const std::string& dirname) {
  char const* begin = dirname.c_str();
  char const* cur = begin;

  if (exists(dirname)) {
    if (isDirectory(dirname)) {
      return;
    } else {
      RAISE(
          kIOError,
          "file '%s' exists but is not a directory",
          dirname.c_str());
    }
  }

  for (cur = begin; *cur == '/'; ++cur);

  while ((cur = strchr(cur, '/'))) {
    std::string path(begin, cur);
    cur++;

    if (exists(path)) {
      if (isDirectory(path)) {
        continue;
      } else {
        RAISE(
            kIOError,
            "file '%s' exists but is not a directory",
            path.c_str());
      }
    }

    mkdir(path);
  }

  mkdir(dirname);
}

std::string FileUtil::joinPaths(const std::string& p1, const std::string p2) {
  String p1_stripped = p1;
  fviz::StringUtil::stripTrailingSlashes(&p1_stripped);
  String p2_stripped = p2;
  fviz::StringUtil::stripTrailingSlashes(&p2_stripped);
  return p1_stripped + "/" + p2_stripped;
}

void FileUtil::ls(
    const std::string& dirname,
    std::function<bool(const std::string&)> callback) {
  if (exists(dirname)) {
    if (!isDirectory(dirname)) {
      RAISE(
          kIOError,
          "file '%s' exists but is not a directory",
          dirname.c_str());
    }
  } else {
    RAISE(
        kIOError,
        "file '%s' does not exist",
        dirname.c_str());
  }

  auto dir = opendir(dirname.c_str());

  if (dir == nullptr) {
    RAISE_ERRNO("opendir(%s) failed", dirname.c_str());
  }

  struct dirent* entry;
  while ((entry = readdir(dir)) != NULL) {
#if defined(__APPLE__)
    size_t namlen = entry->d_namlen;
#else
    size_t namlen = strlen(entry->d_name);
#endif
    if (namlen < 1 || *entry->d_name == '.') {
      continue;
    }

    if (!callback(std::string(entry->d_name, namlen))) {
      break;
    }
  }

  closedir(dir);
}

void FileUtil::rm(const std::string& filename) {
  unlink(filename.c_str());
}

void FileUtil::mv(const std::string& src, const std::string& dst) {
  if (::rename(src.c_str(), dst.c_str()) < 0) {
    RAISE_ERRNO(kIOError, "rename(%s, %s) failed", src.c_str(), dst.c_str());
  }
}

void FileUtil::truncate(const std::string& filename, size_t new_size) {
  if (::truncate(filename.c_str(), new_size) < 0) {
    RAISE_ERRNO(kIOError, "truncate(%s) failed", filename.c_str());
  }
}

Buffer FileUtil::read(const std::string& filename) {
  auto file = File::openFile(filename, File::O_READ);
  Buffer buf(file.size());
  file.read(&buf);
  return buf;
}

void FileUtil::write(const std::string& filename, const Buffer& data) {
  auto file = File::openFile(
      filename,
      File::O_WRITE | File::O_CREATEOROPEN | File::O_TRUNCATE);

  file.write(data);
}

void FileUtil::cp(const std::string& src, const std::string& destination) {
  RAISE(kNotYetImplementedError);
}

size_t FileUtil::du_c(const std::string& path) {
  size_t size = 0;

  FileUtil::ls(path, [&path, &size] (const String& file) -> bool {
    auto filename = FileUtil::joinPaths(path, file);

    if (FileUtil::isDirectory(filename)) {
      size += FileUtil::du_c(filename);
    } else {
      size += FileUtil::size(filename);
    }

    return true;
  });

  return size;
}

ReturnCode read_file(const std::string& path, std::string* data) {
  std::stringstream buffer;
  {
    std::ifstream file(path);
    if (!file.is_open()) {
      return errorf(
          ERROR,
          "error while reading file: '{}': {}",
          path,
          strerror(errno));
    }

    buffer << file.rdbuf();
  }

  *data = buffer.str();
  return OK;
}

}
