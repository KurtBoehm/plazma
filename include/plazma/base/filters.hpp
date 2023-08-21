#ifndef INCLUDE_PLAZMA_BASE_FILTERS_HPP
#define INCLUDE_PLAZMA_BASE_FILTERS_HPP

#include <array>
#include <cstdlib>

#include <lzma.h>

namespace plazma {
struct Filters {
  Filters() {
    data_[LZMA_FILTERS_MAX].id = LZMA_VLI_UNKNOWN;
  }
  Filters(const Filters&) = delete;
  Filters(Filters&&) = delete;
  Filters& operator=(const Filters&) = delete;
  Filters& operator=(Filters&&) = delete;
  ~Filters() {
    for (auto& filter : data_) {
      std::free(filter.options);
    }
  }

  lzma_filter* data() {
    return data_.data();
  }

private:
  std::array<lzma_filter, LZMA_FILTERS_MAX + 1> data_{};
};
} // namespace plazma

#endif // INCLUDE_PLAZMA_BASE_FILTERS_HPP
