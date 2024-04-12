#ifndef INCLUDE_PLAZMA_DECODE_READER_HPP
#define INCLUDE_PLAZMA_DECODE_READER_HPP

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstring>
#include <filesystem>
#include <span>

#include <fmt/core.h>
#include <lzma.h>

#include "thesauros/containers.hpp"
#include "thesauros/io.hpp"
#include "thesauros/utility.hpp"

#include "plazma/base.hpp"
#include "plazma/decode/read-index.hpp"

namespace plazma {
struct Reader : public thes::FileReader {
  struct BlockSentinel {};

  struct BlockIter {
    explicit BlockIter(Reader& reader, const lzma_index* index) : reader_(reader) {
      lzma_index_iter_init(&it_, index);
    }

    [[nodiscard]] lzma_index_iter raw() const {
      return it_;
    }
    [[nodiscard]] lzma_index_iter& raw() {
      return it_;
    }

    BlockIter& operator++() {
      assert(!is_end_);
      const auto ret = lzma_index_iter_next(&it_, LZMA_INDEX_ITER_NONEMPTY_BLOCK);
      if (ret == 1) {
        is_end_ = true;
      }
      return *this;
    }

    Block operator*() {
      assert(!is_end_);
      return Block(reader_, it_);
    }

    thes::ArrowProxy<Block> operator->() {
      assert(!is_end_);
      return {**this};
    }

    friend bool operator==(const BlockIter& iter, BlockSentinel /*sentinel*/) {
      return iter.is_end_;
    }

  private:
    Reader& reader_;
    lzma_index_iter it_{};
    bool is_end_{false};
  };

  explicit Reader(const std::filesystem::path& path)
      : thes::FileReader(path), index_([this] {
          thes::DynamicBuffer header(LZMA_STREAM_HEADER_SIZE);
          read(std::span{header.data(), LZMA_STREAM_HEADER_SIZE});

          lzma_stream_flags flags;
          lzma_ret ret = lzma_stream_header_decode(&flags, header.data_u8());
          if (ret == LZMA_FORMAT_ERROR) {
            throw Exception(
              "Magic bytes don't match, thus the given buffer cannot be Stream Header.");
          }
          if (ret == LZMA_DATA_ERROR) {
            throw Exception("CRC32 doesn't match, thus the header is corrupt.");
          }
          if (ret == LZMA_OPTIONS_ERROR) {
            throw Exception("Unsupported options are present in the header.");
          }
          if (ret != LZMA_OK) {
            throw Exception(fmt::format("Invalid header: {}", ret));
          }

          return read_index(*this);
        }()) {}
  Reader(const Reader&) = delete;
  Reader(Reader&&) = delete;
  Reader& operator=(const Reader&) = delete;
  Reader& operator=(Reader&&) = delete;

  ~Reader() {
    lzma_index_end(index_, nullptr);
  }

  [[nodiscard]] BlockIter iter_at(lzma_vli off) {
    BlockIter iter(*this, index_);
    const auto ret = lzma_index_iter_locate(&iter.raw(), off);
    if (ret == 1) {
      throw Exception("Locating block failed!");
    }
    return iter;
  }

  template<typename T>
  requires std::is_trivial_v<T>
  void load_segment(std::size_t off, std::span<T> out) {
    const auto offset = off * sizeof(T);
    const auto size = out.size() * sizeof(T);
    auto* data = reinterpret_cast<std::byte*>(out.data());

    auto it_end = end();
    const auto out_end = offset + size;
    thes::DynamicBuffer scratch{};
    thes::DynamicBuffer buf{};
    for (auto it = iter_at(static_cast<lzma_vli>(offset)); it != it_end and it->uoff() < out_end;
         ++it) {
      it->decompress(scratch, buf);
      const auto common_begin = std::max(offset, it->uoff());
      const auto buf_begin = common_begin - it->uoff();
      const auto out_begin = common_begin - offset;
      const auto num = std::min(it->uend(), out_end) - common_begin;
      std::memcpy(data + out_begin, buf.data() + buf_begin, num);
    }
  }

  [[nodiscard]] BlockIter begin() {
    return BlockIter(*this, index_);
  }
  [[nodiscard]] BlockSentinel end() {
    return BlockSentinel{};
  }

  [[nodiscard]] std::size_t uncompressed_size() {
    return lzma_index_uncompressed_size(index_);
  }
  [[nodiscard]] std::size_t block_count() {
    return lzma_index_block_count(index_);
  }

private:
  lzma_index* index_;
};
} // namespace plazma

#endif // INCLUDE_PLAZMA_DECODE_READER_HPP
