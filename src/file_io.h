///////////////////////////////////////////////////////////////////////////////
//
/// \file       file_io.h
/// \brief      I/O types and functions
//
//  Author:     Lasse Collin
//
//  This file has been put into the public domain.
//  You can do whatever you want with this file.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef SRC_FILE_IO_H
#define SRC_FILE_IO_H

#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/stat.h>

// Some systems have suboptimal BUFSIZ. Use a bit bigger value on them.
// We also need that IO_BUFFER_SIZE is a multiple of 8 (sizeof(uint64_t))
#if BUFSIZ <= 1024
#	define IO_BUFFER_SIZE 8192
#else
#	define IO_BUFFER_SIZE (BUFSIZ & ~7U)
#endif


#define _(MSG) MSG

/// is_sparse() accesses the buffer as uint64_t for maximum speed.
/// The u32 and u64 members must only be access through this union
/// to avoid strict aliasing violations. Taking a pointer of u8
/// should be fine as long as uint8_t maps to unsigned char which
/// can alias anything.
typedef union {
	uint8_t u8[IO_BUFFER_SIZE];
	uint32_t u32[IO_BUFFER_SIZE / sizeof(uint32_t)];
	uint64_t u64[IO_BUFFER_SIZE / sizeof(uint64_t)];
} io_buf;


typedef struct {
	const uint8_t* src_curr;
	const uint8_t* src_end;
	FILE* dst;
} file_pair;


/// \brief      Open the source file
extern file_pair *io_open_src(const char *src_name);


/// \brief      Open the destination file
extern bool io_open_dest(file_pair *pair);


/// \brief      Closes the file descriptors and frees possible allocated memory
///
/// The success argument determines if source or destination file gets
/// unlinked:
///  - false: The destination file is unlinked.
///  - true: The source file is unlinked unless writing to stdout or --keep
///    was used.
extern void io_close(file_pair *pair, bool success);


/// \brief      Reads from the source file to a buffer
///
/// \param      pair    File pair having the source file open for reading
/// \param      buf     Destination buffer to hold the read data
/// \param      size    Size of the buffer; assumed be smaller than SSIZE_MAX
///
/// \return     On success, number of bytes read is returned. On end of
///             file zero is returned and pair->src_eof set to true.
///             On error, SIZE_MAX is returned and error message printed.
extern size_t io_read(file_pair *pair, io_buf *buf, size_t size);


/// \brief      Writes a buffer to the destination file
///
/// \param      pair    File pair having the destination file open for writing
/// \param      buf     Buffer containing the data to be written
/// \param      size    Size of the buffer; assumed be smaller than SSIZE_MAX
///
/// \return     On success, zero is returned. On error, -1 is returned
///             and error message printed.
extern bool io_write(file_pair *pair, const io_buf *buf, size_t size);

#endif // SRC_FILE_IO_H
