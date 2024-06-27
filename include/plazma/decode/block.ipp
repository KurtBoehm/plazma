#ifndef INCLUDE_PLAZMA_DECODE_BLOCK_IPP
#define INCLUDE_PLAZMA_DECODE_BLOCK_IPP

#include <span>

#include <fmt/core.h>
#include <lzma.h>

#include "thesauros/containers.hpp"

#include "plazma/base/block.hpp"
#include "plazma/base/exception.hpp"
#include "plazma/base/filters.hpp"
#include "plazma/base/stream.hpp"
#include "plazma/decode/decode.hpp"
#include "plazma/decode/reader.hpp"

namespace plazma {
void Block::decompress(thes::DynamicBuffer& scratch, thes::DynamicBuffer& out) {
  // read the header
  lzma_block block{};
  block.version = 0;
  block.check = check();

  Filters filters{};
  block.filters = filters.data();

  reader_.pread(scratch, 1, static_cast<long>(coff()));
  block.header_size = lzma_block_header_size_decode(scratch[0]);
  scratch.resize(block.header_size);
  reader_.pread(std::span{scratch.data() + 1, block.header_size - 1},
                static_cast<long>(coff()) + 1);

  lzma_ret err = lzma_block_header_decode(&block, nullptr, scratch.data_u8());
  if (err == LZMA_OPTIONS_ERROR) {
    throw Exception(
      "The Block Header specifies some unsupported options such as unsupported filters.");
  }
  if (err == LZMA_DATA_ERROR) {
    throw Exception("Block Header is corrupt, for example, the CRC32 doesn't match.");
  }
  if (err == LZMA_PROG_ERROR) {
    throw Exception("Invalid arguments.");
  }
  if (err != LZMA_OK) {
    throw Exception(fmt::format("Error in block header: {}", err));
  }

  // decode the block
  Stream s{};
  if (lzma_block_decoder(&s, &block) != LZMA_OK) {
    throw Exception("error initializing block decoder");
  }

  out.resize(usize());
  s.next_out = out.data_u8();
  s.avail_out = out.size();
  decode(s, reader_, scratch, coff() + block.header_size);
}
} // namespace plazma

#endif // INCLUDE_PLAZMA_DECODE_BLOCK_IPP
