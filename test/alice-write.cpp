#include <filesystem>
#include <iostream>
#include <span>
#include <string>
#include <utility>

#include "thesauros/io.hpp"
#include "thesauros/test.hpp"

#include "plazma/plazma.hpp"

int main(int /*argc*/, const char* const* const argv) {
  const auto base_path = std::filesystem::canonical(std::filesystem::path{argv[0]}.parent_path());
  const auto md_path = base_path / "alice.md";
  const auto xz_path = base_path / "alice-out.md.xz";

  thes::FileReader md_reader{md_path};
  const auto md_size = md_reader.size();
  std::string md_str(md_size + 1, '\0');
  md_reader.pread(std::span{md_str.data(), md_size}, 0);

  {
    plazma::Writer xz_writer{xz_path};
    xz_writer.write(std::span{std::as_const(md_str).data(), md_size});
  }

  plazma::Reader xz_reader{xz_path};
  std::cout << "block count: " << xz_reader.block_count() << '\n';
  const auto xz_size = xz_reader.uncompressed_size();
  std::string xz_str(xz_size + 1, '\0');
  xz_reader.load_segment(0, std::span{xz_str.data(), xz_size});

  THES_ASSERT(md_str == xz_str);
  THES_ASSERT(xz_reader.size() == 45704);
  THES_ASSERT(xz_reader.uncompressed_size() == 147251);
}
