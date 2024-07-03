#include <cstddef>
#include <filesystem>
#include <iostream>
#include <span>
#include <string>

#include "thesauros/thesauros.hpp"

#include "plazma/plazma.hpp"

int main(int /*argc*/, const char* const* const argv) {
  const auto base_path = std::filesystem::canonical(std::filesystem::path{argv[0]}.parent_path());
  const auto md_path = base_path / "alice.md";
  const auto xz_path = base_path / "alice.md.xz";

  THES_ASSERT(std::filesystem::exists(md_path));
  THES_ASSERT(std::filesystem::exists(xz_path));

  thes::FileReader md_reader{md_path};
  const auto md_size = md_reader.size();
  std::string md_str(md_size + 1, '\0');
  md_reader.pread(std::span{md_str.data(), md_size}, 0);

  plazma::Reader xz_reader{xz_path};
  std::cout << "block count: " << xz_reader.block_count() << '\n';
  const auto xz_size = xz_reader.uncompressed_size();
  std::string xz_str(xz_size + 1, '\0');
  xz_reader.load_segment(0, std::span{xz_str.data(), xz_size});

  THES_ASSERT(md_str == xz_str);

  THES_ASSERT(xz_reader.size() == 92172);
  THES_ASSERT(xz_reader.uncompressed_size() == 147251);

  for (std::size_t thread_num = 1; thread_num <= 8; ++thread_num) {
    std::cout << thread_num << '\n';
    std::string str(xz_size + 1, '\0');

    thes::FixedStdThreadPool pool(thread_num);
    thes::UniformIndexSegmenter seg{xz_size, pool.size()};
    pool.execute([&](const std::size_t idx) {
      plazma::Reader reader{xz_path};
      const auto iota = seg.segment_range(idx);
      const auto begin = iota.begin_value();
      reader.load_segment(begin, std::span{str.data() + begin, iota.size()});
    });

    THES_ASSERT(md_str == str);
  }
}
