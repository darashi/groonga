/* -*- c-basic-offset: 2 -*- */
/* Copyright(C) 2011 Brazil

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License version 2.1 as published by the Free Software Foundation.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef GRN_DAT_COMMON_HPP_
#define GRN_DAT_COMMON_HPP_

#ifndef _MSC_VER
 #include <stdint.h>
#endif  // _MSC_VER

#include <cstddef>
#include <exception>

#ifdef _DEBUG
 #include <iostream>
#endif  // _DEBUG

namespace grn {
namespace dat {

#ifdef _MSC_VER
typedef unsigned __int8  UInt8;
typedef unsigned __int16 UInt16;
typedef unsigned __int32 UInt32;
typedef unsigned __int64 UInt64;
#else  // _MSC_VER
typedef ::uint8_t UInt8;
typedef ::uint16_t UInt16;
typedef ::uint32_t UInt32;
typedef ::uint64_t UInt64;
#endif  // _MSC_VER

#ifndef UINT8_MAX
const UInt8 UINT8_MAX = static_cast<UInt8>(0xFFU);
#endif  // UINT8_MAX

#ifndef UINT8_MAX
const UInt16 UINT16_MAX = static_cast<UInt16>(0xFFFFU);
#endif  // UINT16_MAX

#ifndef UINT8_MAX
const UInt32 UINT32_MAX = static_cast<UInt32>(0xFFFFFFFFU);
#endif  // UINT32_MAX

#ifndef UINT8_MAX
const UInt64 UINT64_MAX = static_cast<UInt64>(0xFFFFFFFFFFFFFFFFULL);
#endif  // UINT64_MAX

// If a key is a prefix of another key, such a key is associated with a special
// terminal node which has TERMINAL_LABEL.
const UInt16 TERMINAL_LABEL  = 0x100;
const UInt16 MIN_LABEL       = '\0';
const UInt16 MAX_LABEL       = TERMINAL_LABEL;
const UInt32 INVALID_LABEL   = 0x1FF;
const UInt32 LABEL_MASK      = 0x1FF;

// The MSB of BASE is used to represent whether the node is a linker node or
// not and the other 31 bits represent the offset to its child nodes. So, the
// number of nodes is limited to 2^31.
const UInt32 ROOT_NODE_ID    = 0;
const UInt32 MAX_NODE_ID     = 0x7FFFFFFF;
const UInt32 MAX_NUM_NODES   = MAX_NODE_ID + 1;
const UInt32 INVALID_NODE_ID = MAX_NODE_ID + 1;

// 0 is reserved for non-linker leaf nodes. For example, the root node of an
// initial double-array is a non-linker leaf node.
const UInt32 MAX_OFFSET      = MAX_NODE_ID;
const UInt32 INVALID_OFFSET  = 0;

// Phantom nodes are managed in each block because siblings are always put in
// the same block.
const UInt32 BLOCK_SIZE      = 0x200;
const UInt32 BLOCK_MASK      = 0x1FF;
const UInt32 MAX_BLOCK_ID    = MAX_NODE_ID / BLOCK_SIZE;
const UInt32 MAX_NUM_BLOCKS  = MAX_BLOCK_ID + 1;

// Blocks are divided by their levels, which indicate how easily update
// operations can find a good offset in them. The level of a block rises when
// find_offset() fails in that block many times. MAX_FAILURE_COUNT is the
// threshold. Also, in order to limit the time cost, find_offset() scans at
// most MAX_BLOCK_COUNT blocks.
// Larger parameters bring more chances of finding good offsets but it leads to
// more node renumberings, which are costly operations, and thus results in
// a degradation of space/time efficiencies.
const UInt32 MAX_FAILURE_COUNT  = 4;
const UInt32 MAX_BLOCK_COUNT    = 16;
const UInt32 MAX_BLOCK_LEVEL    = 5;

// Blocks in the same level compose a doubly linked list. The entry block of
// a linked list is called a leader. INVALID_LEADER means that a linked list is
// empty and there exists no leader.
const UInt32 INVALID_LEADER     = 0x7FFFFFFF;

const UInt32 MIN_KEY_ID      = 1;
const UInt32 MAX_KEY_ID      = MAX_NODE_ID;
const UInt32 INVALID_KEY_ID  = 0;

// A key length is represented as a 12-bit unsigned integer in Key.
// A key ID is represented as a 28-bit unsigned integer in Key.
const UInt32 MAX_KEY_LENGTH  = (1U << 12) - 1;
const UInt32 MAX_NUM_KEYS    = (1U << 28) - 1;

const UInt64 MIN_FILE_SIZE              = 1 << 16;
const UInt64 DEFAULT_FILE_SIZE          = 1 << 20;
const UInt64 MAX_FILE_SIZE              = (UInt64)1 << 40;
const double DEFAULT_NUM_NODES_PER_KEY  = 4.0;
const double DEFAULT_AVERAGE_KEY_LENGTH = 16.0;
const UInt32 MAX_KEY_BUF_SIZE           = 0x80000000U;
const UInt32 MAX_TOTAL_KEY_LENGTH       = 0xFFFFFFFFU;

const UInt32 ID_RANGE_CURSOR      = 0x00001;
const UInt32 KEY_RANGE_CURSOR     = 0x00002;
const UInt32 PREFIX_CURSOR        = 0x00004;
const UInt32 PREDICTIVE_CURSOR    = 0x00008;
const UInt32 CURSOR_TYPE_MASK     = 0x000FF;

const UInt32 ASCENDING_CURSOR     = 0x00100;
const UInt32 DESCENDING_CURSOR    = 0x00200;
const UInt32 CURSOR_ORDER_MASK    = 0x00F00;

const UInt32 EXCEPT_LOWER_BOUND   = 0x01000;
const UInt32 EXCEPT_UPPER_BOUND   = 0x02000;
const UInt32 EXCEPT_EXACT_MATCH   = 0x04000;
const UInt32 CURSOR_OPTIONS_MASK  = 0xFF000;

enum ErrorCode {
  PARAM_ERROR      = -1,
  IO_ERROR         = -2,
  FORMAT_ERROR     = -3,
  MEMORY_ERROR     = -4,
  SIZE_ERROR       = -5,
  UNEXPECTED_ERROR = -6
};

class Exception : public std::exception {
 public:
  Exception() throw()
      : file_(""),
        line_(-1),
        what_("") {}
  Exception(const char *file, int line, const char *what) throw()
      : file_(file),
        line_(line),
        what_((what != NULL) ? what : "") {}
  Exception(const Exception &ex) throw()
      : file_(ex.file_),
        line_(ex.line_),
        what_(ex.what_) {}
  virtual ~Exception() throw() {}

  virtual Exception &operator=(const Exception &ex) throw() {
    file_ = ex.file_;
    line_ = ex.line_;
    what_ = ex.what_;
    return *this;
  }

  virtual ErrorCode code() const throw() = 0;
  virtual const char *file() const throw() {
    return file_;
  }
  virtual int line() const throw() {
    return line_;
  }
  virtual const char *what() const throw() {
    return what_;
  }

 private:
  const char *file_;
  int line_;
  const char *what_;
};

template <ErrorCode T>
class Error : public Exception {
 public:
  Error() throw()
      : Exception() {}
  Error(const char *file, int line, const char *what) throw()
      : Exception(file, line, what) {}
  Error(const Error &ex) throw()
      : Exception(ex) {}
  virtual ~Error() throw() {}

  virtual Error &operator=(const Error &ex) throw() {
      *static_cast<Exception *>(this) = ex;
      return *this;
  }

  virtual ErrorCode code() const throw() {
    return T;
  }
};

typedef Error<PARAM_ERROR> ParamError;
typedef Error<IO_ERROR> IOError;
typedef Error<FORMAT_ERROR> FormatError;
typedef Error<MEMORY_ERROR> MemoryError;
typedef Error<SIZE_ERROR> SizeError;
typedef Error<UNEXPECTED_ERROR> UnexpectedError;

#define GRN_DAT_INT_TO_STR(value) \
    #value
#define GRN_DAT_LINE_TO_STR(line) \
    GRN_DAT_INT_TO_STR(line)
#define GRN_DAT_LINE_STR \
    GRN_DAT_LINE_TO_STR(__LINE__)

#define GRN_DAT_THROW(code, msg) \
    (throw grn::dat::Error<code>(__FILE__, __LINE__, \
     __FILE__ ":" GRN_DAT_LINE_STR ": " #code ": " msg))
#define GRN_DAT_THROW_IF(code, cond) \
    (void)((!(cond)) || (GRN_DAT_THROW(code, #cond), 0))

#ifdef _DEBUG
 #define GRN_DAT_DEBUG_THROW_IF(cond) \
     GRN_DAT_THROW_IF(grn::dat::UNEXPECTED_ERROR, cond)
 #define GRN_DAT_DEBUG_LOG(var) \
     (std::clog << __FILE__ ":" GRN_DAT_LINE_STR ": " #var ": " \
                << (var) << std::endl)
#else  // _DEBUG
 #define GRN_DAT_DEBUG_THROW_IF(cond)
 #define GRN_DAT_DEBUG_LOG(var)
#endif  // _DEBUG

}  // namespace dat
}  // namespace grn

#endif  // GRN_DAT_COMMON_HPP_
