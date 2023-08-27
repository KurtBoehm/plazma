#ifndef INCLUDE_PLAZMA_ENCODE_WRITER_HPP
#define INCLUDE_PLAZMA_ENCODE_WRITER_HPP

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <optional>
#include <span>
#include <utility>

#include <lzma.h>

#include "thesauros/io.hpp"

namespace plazma {
// Based on doc/04_compress_easy_mt.c
struct Writer : public thes::FileWriter {
  explicit Writer(const std::filesystem::path& path, std::optional<std::uint64_t> block_size = {},
                  std::optional<std::uint32_t> thread_num = {})
      : thes::FileWriter(path) {
    const int preset = 9;
    // Options for LZMA1 or LZMA2 in case we are using a preset.
    lzma_options_lzma opt_lzma;
    lzma_lzma_preset(&opt_lzma, preset);

    // Use an LZMA2 filter with the given preset.
    std::array<lzma_filter, LZMA_FILTERS_MAX + 1> filters{};
    filters[0].id = LZMA_FILTER_LZMA2;
    filters[0].options = &opt_lzma;

    // Terminate the filter options array.
    filters[1].id = LZMA_VLI_UNKNOWN;

    // bool success = init_encoder(&strm_);
    // The threaded encoder takes the options as pointer to
    // a lzma_mt structure.
    const lzma_mt mt{
      // No flags are needed.
      .flags = 0,

      // Detect how many threads the CPU supports.
      // If the number of CPU cores/threads cannot be detected,
      // use one thread. Note that this isn't the same as the normal
      // single-threaded mode as this will still split the data into
      // blocks and use more RAM than the normal single-threaded mode.
      .threads = thread_num.value_or(std::max(lzma_cputhreads(), std::uint32_t{1})),

      // Let liblzma determine a sane block size.
      .block_size = block_size.value_or(0),

      // Use no timeout for lzma_code() calls by setting timeout
      // to zero. That is, sometimes lzma_code() might block for
      // a long time (from several seconds to even minutes).
      // If this is not OK, for example due to progress indicator
      // needing updates, specify a timeout in milliseconds here.
      // See the documentation of lzma_mt in lzma/container.h for
      // information how to choose a reasonable timeout.
      .timeout = 0,

      // Use the filters defined above.
      .preset = 0,
      .filters = filters.data(),

      // Use CRC64 for integrity checking.
      .check = LZMA_CHECK_CRC64,
    };

    // Initialize the threaded encoder.
    const lzma_ret ret = lzma_stream_encoder_mt(&strm_, &mt);

    switch (ret) {
    case LZMA_OK: break;
    case LZMA_MEM_ERROR: throw Exception("Memory allocation failed");
    case LZMA_OPTIONS_ERROR:
      // We are no longer using a plain preset so this error
      // message has been edited accordingly compared to
      // 01_compress_easy.c.
      throw Exception("Specified filter chain is not supported");
    case LZMA_UNSUPPORTED_CHECK: throw Exception("Specified integrity check is not supported");
    default: throw Exception("Unknown error ", ret, ", possibly a bug");
    }
  }

  Writer(const Writer&) = delete;
  Writer(Writer&&) = delete;
  Writer& operator=(const Writer&) = delete;
  Writer& operator=(Writer&&) = delete;

  ~Writer() {
    // Free the memory allocated for the encoder. If we were encoding
    // multiple files, this would only need to be done after the last
    // file. See 02_decompress.c for handling of multiple files.
    //
    // It is OK to call lzma_end() multiple times or when it hasn't been
    // actually used except initialized with LZMA_STREAM_INIT.
    lzma_end(&strm_);
  }

  template<typename T>
  requires std::is_trivial_v<T>
  void write(std::span<const T> span) {
    // This will be LZMA_RUN until the end of the input file is reached.
    // This tells lzma_code() when there will be no more input.
    lzma_action action = LZMA_RUN;

    // Buffer to temporarily hold uncompressed input
    // and compressed output.
    std::array<uint8_t, BUFSIZ> outbuf{};

    // Initialize the input and output pointers. Initializing next_in
    // and avail_in isn't really necessary when we are going to encode
    // just one file since LZMA_STREAM_INIT takes care of initializing
    // those already. But it doesn't hurt much and it will be needed
    // if encoding more than one file like we will in 02_decompress.c.
    //
    // While we don't care about strm_.total_in or strm_.total_out in this
    // example, it is worth noting that initializing the encoder will
    // always reset total_in and total_out to zero. But the encoder
    // initialization doesn't touch next_in, avail_in, next_out, or
    // avail_out.
    strm_.next_in = nullptr;
    strm_.avail_in = 0;
    strm_.next_out = outbuf.data();
    strm_.avail_out = BUFSIZ;

    const auto* current = reinterpret_cast<const std::uint8_t*>(span.data());
    const auto* end = current + span.size();

    // Loop until the file has been successfully compressed or until
    // an error occurs.
    while (true) {
      // Fill the input buffer if it is empty.
      if (strm_.avail_in == 0 && current != end) {
        strm_.next_in = current;
        const auto read_size = std::min<std::size_t>(end - current, BUFSIZ);
        strm_.avail_in = read_size;
        current += read_size;

        // Once the end of the input file has been reached,
        // we need to tell lzma_code() that no more input
        // will be coming and that it should finish the
        // encoding.
        if (current == end) {
          action = LZMA_FINISH;
        }
      }

      // Tell liblzma do the actual encoding.
      //
      // This reads up to strm_.avail_in bytes of input starting
      // from strm_.next_in. avail_in will be decremented and
      // next_in incremented by an equal amount to match the
      // number of input bytes consumed.
      //
      // Up to strm_.avail_out bytes of compressed output will be
      // written starting from strm_.next_out. avail_out and next_out
      // will be incremented by an equal amount to match the number
      // of output bytes written.
      //
      // The encoder has to do internal buffering, which means that
      // it may take quite a bit of input before the same data is
      // available in compressed form in the output buffer.
      lzma_ret ret = lzma_code(&strm_, action);

      // If the output buffer is full or if the compression finished
      // successfully, write the data from the output buffer to
      // the output file.
      if (strm_.avail_out == 0 || ret == LZMA_STREAM_END) {
        // When lzma_code() has returned LZMA_STREAM_END,
        // the output buffer is likely to be only partially
        // full. Calculate how much new data there is to
        // be written to the output file.
        size_t write_size = BUFSIZ - strm_.avail_out;

        thes::FileWriter::write(std::span{std::as_const(outbuf).data(), write_size});

        // Reset next_out and avail_out.
        strm_.next_out = outbuf.data();
        strm_.avail_out = BUFSIZ;
      }

      // Normally the return value of lzma_code() will be LZMA_OK
      // until everything has been encoded.
      if (ret != LZMA_OK) {
        // Once everything has been encoded successfully, the
        // return value of lzma_code() will be LZMA_STREAM_END.
        //
        // It is important to check for LZMA_STREAM_END. Do not
        // assume that getting ret != LZMA_OK would mean that
        // everything has gone well.
        if (ret == LZMA_STREAM_END) {
          break;
        }

        // It's not LZMA_OK nor LZMA_STREAM_END,
        // so it must be an error code. See lzma/base.h
        // (src/liblzma/api/lzma/base.h in the source package
        // or e.g. /usr/include/lzma/base.h depending on the
        // install prefix) for the list and documentation of
        // possible values. Most values listen in lzma_ret
        // enumeration aren't possible in this example.
        switch (ret) {
        case LZMA_MEM_ERROR: throw Exception("Memory allocation failed");
        case LZMA_DATA_ERROR:
          // This error is returned if the compressed
          // or uncompressed size get near 8 EiB
          // (2^63 bytes) because that's where the .xz
          // file format size limits currently are.
          // That is, the possibility of this error
          // is mostly theoretical unless you are doing
          // something very unusual.
          //
          // Note that strm_.total_in and strm_.total_out
          // have nothing to do with this error. Changing
          // those variables won't increase or decrease
          // the chance of getting this error.
          throw Exception("File size limits exceeded");
        default:
          // This is most likely LZMA_PROG_ERROR, but
          // if this program is buggy (or liblzma has
          // a bug), it may be e.g. LZMA_BUF_ERROR or
          // LZMA_OPTIONS_ERROR too.
          //
          // It is inconvenient to have a separate
          // error message for errors that should be
          // impossible to occur, but knowing the error
          // code is important for debugging. That's why
          // it is good to print the error code at least
          // when there is no good error message to show.
          throw Exception("Unknown error ", ret, ", possibly a bug");
        }
      }
    }
  }

private:
  lzma_stream strm_ = LZMA_STREAM_INIT;
};
} // namespace plazma

#endif // INCLUDE_PLAZMA_ENCODE_WRITER_HPP
