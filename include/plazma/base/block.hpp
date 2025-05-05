// This file is part of https://github.com/KurtBoehm/plazma.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef INCLUDE_PLAZMA_BASE_BLOCK_HPP
#define INCLUDE_PLAZMA_BASE_BLOCK_HPP

#include <lzma.h>

#include "thesauros/containers.hpp"

namespace plazma {
struct Reader;

struct Block {
  explicit Block(Reader& reader, lzma_index_iter it) : reader_(reader), it_(it) {}

  [[nodiscard]] lzma_index_iter raw() const {
    return it_;
  }

  [[nodiscard]] auto check() const {
    return it_.stream.flags->check;
  }
  [[nodiscard]] auto coff() const {
    return it_.block.compressed_file_offset;
  }
  [[nodiscard]] auto uoff() const {
    return it_.block.uncompressed_file_offset;
  }
  [[nodiscard]] auto csize() const {
    return it_.block.total_size;
  }
  [[nodiscard]] auto usize() const {
    return it_.block.uncompressed_size;
  }
  [[nodiscard]] auto uend() const {
    return uoff() + usize();
  }

  void decompress(thes::DynamicBuffer& scratch, thes::DynamicBuffer& out);
  void decompress(thes::DynamicBuffer& out) {
    thes::DynamicBuffer scratch{};
    decompress(scratch, out);
  }

private:
  Reader& reader_;
  lzma_index_iter it_;
};
} // namespace plazma

#endif // INCLUDE_PLAZMA_BASE_BLOCK_HPP
