#ifndef INCLUDE_PLAZMA_BASE_DEFS_HPP
#define INCLUDE_PLAZMA_BASE_DEFS_HPP

#include <cstddef>

#include <lzma.h>

#include "thesauros/format.hpp"

namespace plazma {
inline constexpr std::size_t chunk_size = 4096;
}

template<>
struct fmt::formatter<lzma_ret> : formatter<fmt::string_view> {
  auto format(lzma_ret ret, fmt::format_context& ctx) const {
    string_view name = "unknown";
    switch (ret) {
    case LZMA_OK: name = "LZMA_OK"; break;
    case LZMA_STREAM_END: name = "LZMA_STREAM_END"; break;
    case LZMA_NO_CHECK: name = "LZMA_NO_CHECK"; break;
    case LZMA_UNSUPPORTED_CHECK: name = "LZMA_UNSUPPORTED_CHECK"; break;
    case LZMA_GET_CHECK: name = "LZMA_GET_CHECK"; break;
    case LZMA_MEM_ERROR: name = "LZMA_MEM_ERROR"; break;
    case LZMA_MEMLIMIT_ERROR: name = "LZMA_MEMLIMIT_ERROR"; break;
    case LZMA_FORMAT_ERROR: name = "LZMA_FORMAT_ERROR"; break;
    case LZMA_OPTIONS_ERROR: name = "LZMA_OPTIONS_ERROR"; break;
    case LZMA_DATA_ERROR: name = "LZMA_DATA_ERROR"; break;
    case LZMA_BUF_ERROR: name = "LZMA_BUF_ERROR"; break;
    case LZMA_PROG_ERROR: name = "LZMA_PROG_ERROR"; break;
    case LZMA_SEEK_NEEDED: name = "LZMA_SEEK_NEEDED"; break;
    default: break;
    }
    return fmt::formatter<fmt::string_view>::format(name, ctx);
  }
};

#endif // INCLUDE_PLAZMA_BASE_DEFS_HPP
