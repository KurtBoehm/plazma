///////////////////////////////////////////////////////////////////////////////
//
/// \file       file_io.c
/// \brief      File opening, unlinking, and closing
//
//  Author:     Lasse Collin
//
//  This file has been put into the public domain.
//  You can do whatever you want with this file.
//
///////////////////////////////////////////////////////////////////////////////

#include "private.h"

#include <fcntl.h>
#include <string.h>

#ifdef TUKLIB_DOSLIKE
#include <io.h>
#else
#include <poll.h>
#endif

#if defined(HAVE_FUTIMES) || defined(HAVE_FUTIMESAT) || defined(HAVE_UTIMES)
#include <sys/time.h>
#elif defined(HAVE__FUTIME)
#include <sys/utime.h>
#elif defined(HAVE_UTIME)
#include <utime.h>
#endif

#ifdef HAVE_CAPSICUM
#ifdef HAVE_SYS_CAPSICUM_H
#include <sys/capsicum.h>
#else
#include <sys/capability.h>
#endif
#endif

#ifndef O_BINARY
#define O_BINARY 0
#endif

#ifndef O_NOCTTY
#define O_NOCTTY 0
#endif

// Using this macro to silence a warning from gcc -Wlogical-op.
#if EAGAIN == EWOULDBLOCK
#define IS_EAGAIN_OR_EWOULDBLOCK(e) ((e) == EAGAIN)
#else
#define IS_EAGAIN_OR_EWOULDBLOCK(e) ((e) == EAGAIN || (e) == EWOULDBLOCK)
#endif

typedef enum {
  IO_WAIT_MORE, // Reading or writing is possible.
  IO_WAIT_ERROR, // Error or user_abort
  IO_WAIT_TIMEOUT, // poll() timed out
} io_wait_ret;

static bool io_write_buf(file_pair* pair, const uint8_t* buf, size_t size);

extern size_t io_read(file_pair* pair, io_buf* buf, size_t size) {
  // We use small buffers here.
  assert(size < SSIZE_MAX);

  size_t eff_size = my_min(size, (size_t)(pair->src_end - pair->src_curr));
  memcpy(buf, pair->src_curr, eff_size);
  pair->src_curr += eff_size;
  return eff_size;
}

static bool io_write_buf(file_pair* pair, const uint8_t* buf, size_t size) {
  assert(size < SSIZE_MAX);
  return fwrite(buf, 1, size, pair->dst) != size;
}

extern bool io_write(file_pair* pair, const io_buf* buf, size_t size) {
  assert(size <= IO_BUFFER_SIZE);
  return io_write_buf(pair, buf->u8, size);
}
