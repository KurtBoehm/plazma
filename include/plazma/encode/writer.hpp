#ifndef INCLUDE_PLAZMA_ENCODE_WRITER_HPP
#define INCLUDE_PLAZMA_ENCODE_WRITER_HPP

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <optional>
#include <span>

#include <lzma.h>

#include "thesauros/io.hpp"
#include "thesauros/utility.hpp"

namespace plazma {
// Based on doc/04_compress_easy_mt.c
struct Writer : public thes::FileWriter {
  static constexpr std::size_t io_buffer_size = (BUFSIZ <= 1024) ? 8192 : (BUFSIZ & ~7U);
  using IoBuf = std::array<uint8_t, io_buffer_size>;

  explicit Writer(const std::filesystem::path& path, const std::optional<thes::u32> preset = {},
                  std::optional<thes::u64> block_size = {},
                  std::optional<thes::u32> thread_num = {})
      : thes::FileWriter(path) {
    lzma_options_lzma opt_lzma;
    if (lzma_lzma_preset(&opt_lzma, preset.value_or(LZMA_PRESET_DEFAULT)) != 0) {
      throw Exception("Getting preset failed!");
    }

    std::array<lzma_filter, LZMA_FILTERS_MAX + 1> filters{{
      {LZMA_FILTER_LZMA2, &opt_lzma},
      {LZMA_VLI_UNKNOWN, nullptr},
    }};

    const lzma_mt mt{
      .flags = 0,
      .threads = thread_num.value_or(lzma_cputhreads()),
      .block_size = block_size.value_or(0),
      .timeout = 0,
      .filters = filters.data(),
      .check = LZMA_CHECK_CRC64,
    };

    if (const lzma_ret ret = lzma_stream_encoder_mt(&strm_, &mt); ret != LZMA_OK) {
      throw Exception("Error ", ret);
    }
  }

  Writer(const Writer&) = delete;
  Writer(Writer&&) = delete;
  Writer& operator=(const Writer&) = delete;
  Writer& operator=(Writer&&) = delete;

  ~Writer() {
    lzma_end(&strm_);
  }

  template<typename T>
  requires std::is_trivial_v<T>
  void write(std::span<const T> span) {
    IoBuf outbuf{};
    strm_.next_in = nullptr;
    strm_.avail_in = 0;
    strm_.next_out = outbuf.data();
    strm_.avail_out = outbuf.size();

    const auto* current = reinterpret_cast<const std::uint8_t*>(span.data());
    const auto* end = current + span.size();

    // Encoder needs to know when we have given all the input to it.
    // The decoders need to know it too when we are using
    // LZMA_CONCATENATED. We need to check for src_eof here, because
    // the first input chunk has been already read if decompressing,
    // and that may have been the only chunk we will read.
    lzma_action action = (current == end) ? LZMA_FINISH : LZMA_RUN;

    while (true) {
      if (strm_.avail_in == 0 && action == LZMA_RUN) {
        strm_.next_in = current;
        const auto read_size = std::min<std::size_t>(end - current, io_buffer_size);
        strm_.avail_in = read_size;
        current += read_size;

        if (current == end) {
          action = LZMA_FINISH;
        }
      }

      lzma_ret ret = lzma_code(&strm_, action);

      if (strm_.avail_out == 0 || ret == LZMA_STREAM_END) {
        const auto write_size = outbuf.size() - strm_.avail_out;
        thes::FileWriter::write(std::span{outbuf.data(), write_size});
        strm_.next_out = outbuf.data();
        strm_.avail_out = outbuf.size();
      }

      if (ret == LZMA_STREAM_END && current == end) {
        break;
      }
      if (ret != LZMA_OK) {
        throw Exception("Error ", ret);
      }
    }
  }

private:
  lzma_stream strm_ = LZMA_STREAM_INIT;
};
} // namespace plazma

#endif // INCLUDE_PLAZMA_ENCODE_WRITER_HPP
