#include <iostream>
#include <span>

#include "thesauros/containers.hpp"
#include "thesauros/io.hpp"

#include "plazma/encode.hpp"

int main(int argc, const char* const* const argv) {
  if (argc != 3) {
    std::cerr << "Usage: " << argv[0] << " <in_file> <out_file>\n";
    return 1;
  }

  thes::DynamicBuffer contents{};
  {
    thes::FileReader in_reader{argv[1]};
    in_reader.pread(contents, in_reader.size(), 0);
  }
  std::cout << "size: " << contents.size() << '\n';

  {
    plazma::Writer xz_writer{argv[2]};
    xz_writer.write(std::span{contents.data(), contents.size()});
  }
}
