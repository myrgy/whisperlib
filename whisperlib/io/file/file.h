// Copyright (c) 2009, Whispersoft s.r.l.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
// * Neither the name of Whispersoft s.r.l. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Author: Cosmin Tudorache

#ifndef __COMMON_IO_FILE_FILE_H__
#define __COMMON_IO_FILE_FILE_H__

#include <unistd.h>
#include <string.h>

#include <string>

#include <whisperlib/base/types.h>
#include <whisperlib/io/buffer/memory_stream.h>

namespace io {

class File {
 public:
  //////////////////////////////////////////////////////////////////////

  enum Access {
    GENERIC_READ,
    GENERIC_WRITE,
    GENERIC_READ_WRITE
  };
  static const char* AccessName(Access);

  //////////////////////////////////////////////////////////////////////

  enum CreationDisposition {
    // Creates a new file, always.
    // If a file exists, the function overwrites the file, clears the existing
    // attributes.
    CREATE_ALWAYS,
    // Creates a new file. The function fails if a specified file exists.
    CREATE_NEW,
    // Opens a file, always.
    // If a file does not exist, the function creates a file as if
    // creation disposition is CREATE_NEW.
    OPEN_ALWAYS,
    // Opens a file.
    // The function fails if the file does not exist.
    // For more information, see the Remarks section of this topic.
    OPEN_EXISTING,
    // Opens a file and truncates it so that its size is zero (0) bytes.
    // The function fails if the file does not exist.
    // The calling process must open the file with the GENERIC_WRITE
    // access right.
    TRUNCATE_EXISTING
  };
  static const char* CreationDispositionName(CreationDisposition);

  //////////////////////////////////////////////////////////////////////

  enum MoveMethod {
    FILE_SET = SEEK_SET,
    FILE_CUR = SEEK_CUR,
    FILE_END = SEEK_END
  };
  static const char* MoveMethodName(MoveMethod);

  //////////////////////////////////////////////////////////////////////

 public:
  File();
  virtual ~File();

  // Convenience function for creating / opening and truncting a file
  static File* CreateFileOrDie(const char* filename);
  static File* TryCreateFile(const char* filename);

  // Convenience function for opening a file for reading
  static File* OpenFileOrDie(const char* filename);
  static File* TryOpenFile(const char* filename);

  bool Open(const string& filename, Access access, CreationDisposition cd);
  void Close();

  void Set(const string& filename, int fd);

  bool is_open() const {
    return fd_ != INVALID_FD_VALUE;
  }

  const string& filename() const {
    return filename_;
  }
  const int fd() const {
    return fd_;
  }

  // Returns current file size. The file must be opened.
  // (Uses local cached variable: size_)
  int64 Size() const;

  //  Get file pointer position relative to file begin.
  //  (Uses local cached variable: position_)
  int64 Position() const;

  // Set file pointer position to the given absolute offset (relative
  // to file begin)
  //  Returns the new position
  int64 SetPosition(int64 distance, MoveMethod move_method = FILE_SET);

  // Truncate the file to the given size (expands or shortens the file).
  // The file pointer is left at the end of file.
  // If pos==-1 => truncates at current position.
  void Truncate(int64 pos = -1);

  // Reads "len" bytes of data from current file pointer position to
  // the given "buf".
  //  [IN/OUT] buf: buffer that receives the read data.
  //  [IN]  len: the number of bytes to read.
  // Returns the number of bytes read. Negative on error.
  int32 Read(void* buf, int32 len);
  // Same read, but to memory stream.
  int32 Read(io::MemoryStream* out, int32 len);

  // Writes "len" bytes of data from "buf" to file at current file
  // pointer position.
  //   buf: buffer that contains the data to be written.
  //   len: number of bytes to write.
  // Returns the number of bytes written. Negative on error
  int32 Write(const void* buf, int32 len);
  // Same write, but from string.
  int32 Write(const string& s);
  // Same write, but from memory stream. If len==-1 then all ms data is written.
  int32 Write(io::MemoryStream* ms, int32 len = -1);

  // Forces a disk flush
  void Flush();

 protected:
  string filename_;   // name of the opened file
  int fd_;            // file descriptor of the opened file

  int64 size_;        // file size
  int64 position_;    // current file pointer position

  //  Retrieve file size from system and set value in size_
  // returns: success status
  int64 UpdateSize();

  // Retrieve file pointer from system and set value in position_
  // returns: success status
  int64 UpdatePosition();

  DISALLOW_EVIL_CONSTRUCTORS(File);
};
}

#endif  // __COMMON_IO_FILE_FILE_H__
