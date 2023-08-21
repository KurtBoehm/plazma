#ifndef INCLUDE_PLAZMA_DECODE_DECODE_HPP
#define INCLUDE_PLAZMA_DECODE_DECODE_HPP

#include <optional>

#include <lzma.h>

#include "thesauros/containers.hpp"
#include "thesauros/io.hpp"

#include "plazma/base/defs.hpp"
#include "plazma/base/exception.hpp"
#include "plazma/base/stream.hpp"

namespace plazma {
inline void decode(Stream& s, thes::FileReader& fh, thes::DynamicBuffer& scratch,
                   std::optional<long> opt_off = std::nullopt) {
  long off = opt_off.value_or(fh.tell());

  lzma_ret err = LZMA_OK;
  s.avail_in = 0;
  while (err != LZMA_STREAM_END) {
    if (s.avail_in == 0) {
      s.avail_in = fh.try_pread(scratch, chunk_size, off);
      off += static_cast<long>(scratch.size());
      s.next_in = scratch.data_u8();
    }
    err = lzma_code(&s, LZMA_RUN);
    if (err != LZMA_OK and err != LZMA_STREAM_END) {
      throw Exception("Error decoding index: ", err);
    }
  }
}
} // namespace plazma

#endif // INCLUDE_PLAZMA_DECODE_DECODE_HPP
