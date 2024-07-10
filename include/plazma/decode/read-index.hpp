#ifndef INCLUDE_PLAZMA_DECODE_READ_INDEX_HPP
#define INCLUDE_PLAZMA_DECODE_READ_INDEX_HPP

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>

#include <lzma.h>

#include "thesauros/containers.hpp"
#include "thesauros/io.hpp"
#include "thesauros/utility.hpp"

#include "plazma/base/defs.hpp"
#include "plazma/base/exception.hpp"
#include "plazma/base/stream.hpp"
#include "plazma/decode/decode.hpp"

namespace plazma {
inline lzma_index* read_index(thes::FileReader& fh) {
  static_assert(chunk_size % 4 == 0);

  lzma_index* idx = nullptr;
  thes::DynamicBuffer buf{};

  fh.seek(0, thes::Seek::end);
  while (true) {
    // Skip any padding.
    lzma_vli pad = 0;

    [&] {
      while (true) {
        const auto p = fh.tell();
        if (p < LZMA_STREAM_HEADER_SIZE) {
          throw Exception("Padding is not allowed at the start!");
        }
        const auto s = std::min(p, static_cast<long>(chunk_size));
        fh.seek(p - s, thes::Seek::set);
        fh.read(buf, static_cast<std::size_t>(s));

        for (std::byte* it = buf.data() + buf.size(); it != buf.data(); it -= 4) {
          auto* f = it - 4;
          if (thes::byte_read<std::uint32_t>(f) != 0) {
            fh.seek(f - buf.data() - static_cast<long>(buf.size()) + 4, thes::Seek::cur);
            return;
          }
          pad += 4;
        }
        fh.seek(p - s, thes::Seek::set);
      }
    }();

    // Read the footer
    lzma_stream_flags flags;
    fh.seek(-LZMA_STREAM_HEADER_SIZE, thes::Seek::cur);

    std::array<thes::u8, LZMA_STREAM_HEADER_SIZE> footer{};
    fh.read(footer);
    lzma_ret err = lzma_stream_footer_decode(&flags, footer.data());
    if (err != LZMA_OK) {
      throw Exception("Bad Footer");
    }
    long npos = fh.tell();

    // Read the index
    fh.seek(-LZMA_STREAM_HEADER_SIZE - static_cast<long>(flags.backward_size), thes::Seek::cur);
    lzma_index* nidx{};
    {
      Stream s{};
      if (lzma_index_decoder(&s, &nidx, UINT64_MAX) != LZMA_OK) {
        throw Exception("Error initializing index decoder");
      }
      decode(s, fh, buf);
    }
    npos -= static_cast<long>(lzma_index_file_size(nidx));

    // Set index params, combine with any later indices
    if (lzma_index_stream_flags(nidx, &flags) != LZMA_OK) {
      throw Exception("Error setting stream flags");
    }
    if (lzma_index_stream_padding(nidx, pad) != LZMA_OK) {
      throw Exception("Error setting stream padding");
    }
    if (idx != nullptr && lzma_index_cat(nidx, idx, nullptr) != LZMA_OK) {
      throw Exception("Error combining indices");
    }
    idx = nidx;

    if (npos == 0) {
      return idx;
    }
    fh.seek(npos, thes::Seek::set);
  }
}
} // namespace plazma

#endif // INCLUDE_PLAZMA_DECODE_READ_INDEX_HPP
