// This file is part of https://github.com/KurtBoehm/plazma.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef INCLUDE_PLAZMA_BASE_STREAM_HPP
#define INCLUDE_PLAZMA_BASE_STREAM_HPP

#include <lzma.h>

namespace plazma {
struct Stream : public lzma_stream {
  Stream() : lzma_stream(LZMA_STREAM_INIT) {}
  Stream(const Stream&) = delete;
  Stream(Stream&&) = delete;
  Stream& operator=(const Stream&) = delete;
  Stream& operator=(Stream&&) = delete;

  ~Stream() {
    lzma_end(this);
  }
};
} // namespace plazma

#endif // INCLUDE_PLAZMA_BASE_STREAM_HPP
