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
