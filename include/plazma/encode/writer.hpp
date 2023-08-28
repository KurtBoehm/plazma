#ifndef INCLUDE_PLAZMA_ENCODE_WRITER_HPP
#define INCLUDE_PLAZMA_ENCODE_WRITER_HPP

#include <cassert>
#include <cstdint>
#include <filesystem>
#include <optional>
#include <span>

#include <lzma.h>

#include "thesauros/io.hpp"

extern "C" {
#include "coder.h"
#include "file_io.h"
}

namespace plazma {
struct Writer : public thes::FileWriter {
  explicit Writer(const std::filesystem::path& path, std::optional<std::uint64_t> block_size = {},
                  std::optional<std::uint32_t> thread_num = {})
      : thes::FileWriter(path) {
    coder_set_preset(9);
    coder_set_compression_settings();
    coder_init(&strm_);
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
    const auto* begin = reinterpret_cast<const std::uint8_t*>(span.data());
    file_pair fp{.src_curr = begin, .src_end = begin + span.size_bytes(), .dst = handle()};
    const bool success = coder_normal(&fp, &strm_);
    assert(success);
  }

private:
  lzma_stream strm_ = LZMA_STREAM_INIT;
};
} // namespace plazma

#endif // INCLUDE_PLAZMA_ENCODE_WRITER_HPP
