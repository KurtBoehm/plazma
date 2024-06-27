#ifndef INCLUDE_PLAZMA_ENCODE_WRITER_HPP
#define INCLUDE_PLAZMA_ENCODE_WRITER_HPP

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <optional>
#include <span>
#include <type_traits>

#include <fmt/core.h>
#include <lzma.h>

#include "thesauros/io.hpp"
#include "thesauros/macropolis.hpp"
#include "thesauros/utility.hpp"

#include "plazma/base.hpp"

namespace plazma {
struct WriterParams {
  const std::optional<thes::u32> preset{};
  std::optional<thes::u64> block_size{};
  std::optional<thes::u32> thread_num{};
};

// Based on doc/04_compress_easy_mt.c
struct Writer : public thes::FileWriter {
  static constexpr std::size_t io_buffer_size = (BUFSIZ <= 1024) ? 8192 : (BUFSIZ & ~7U);
  using IoBuf = std::array<uint8_t, io_buffer_size>;

  explicit Writer(const std::filesystem::path& dst_path, WriterParams params = {})
      : thes::FileWriter(dst_path) {
    lzma_options_lzma opt_lzma{};
    if (lzma_lzma_preset(&opt_lzma, params.preset.value_or(LZMA_PRESET_DEFAULT)) != 0) {
      throw Exception("Getting preset failed!");
    }

    std::array<lzma_filter, LZMA_FILTERS_MAX + 1> filters{{
      {LZMA_FILTER_LZMA2, &opt_lzma},
      {LZMA_VLI_UNKNOWN, nullptr},
    }};

    THES_POLIS_WARNINGS_PUSH(gcc, "-Wmissing-field-initializers")
    const lzma_mt mt{
      .flags = 0,
      .threads = params.thread_num.value_or(lzma_cputhreads()),
      .block_size = params.block_size.value_or(0),
      .timeout = 0,
      .filters = filters.data(),
      .check = LZMA_CHECK_CRC64,
    };
    THES_POLIS_WARNINGS_POP(gcc)

    if (const lzma_ret ret = lzma_stream_encoder_mt(&strm_, &mt); ret != LZMA_OK) {
      throw Exception(fmt::format("Error {}", ret));
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
  requires std::is_trivial_v<std::remove_const_t<T>>
  void write(std::span<T> span) {
    const auto* current = reinterpret_cast<const thes::u8*>(span.data());
    const auto* end = current + span.size();
    IoBuf out_buf{};
    strm_.next_out = out_buf.data();
    strm_.avail_out = out_buf.size();

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

      const lzma_ret ret = lzma_code(&strm_, action);

      if (strm_.avail_out == 0 || ret == LZMA_STREAM_END) {
        thes::FileWriter::write(std::span{out_buf.data(), out_buf.size() - strm_.avail_out});
        strm_.next_out = out_buf.data();
        strm_.avail_out = out_buf.size();
      }

      if (ret == LZMA_STREAM_END && current == end) {
        break;
      }
      if (ret != LZMA_OK) {
        throw Exception(fmt::format("Error {}", ret));
      }
    }
  }

private:
  lzma_stream strm_ = LZMA_STREAM_INIT;
};
} // namespace plazma

#endif // INCLUDE_PLAZMA_ENCODE_WRITER_HPP
