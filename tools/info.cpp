// This file is part of https://github.com/KurtBoehm/plazma.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include <cstdlib>
#include <filesystem>
#include <iostream>

#include "thesauros/format.hpp"

#include "plazma/plazma.hpp"

int main(int argc, const char** argv) {
  if (argc < 2) {
    fmt::print(stderr, "At least one path is required!\n");
    return EXIT_FAILURE;
  }

  double ratio_sum = 0;
  for (int i = 1; i < argc; ++i) {
    const std::filesystem::path p{argv[i]};
    if (!std::filesystem::exists(p)) {
      std::cerr << p << " does not exist!\n";
    }

    plazma::Reader reader(p);
    const auto csize = reader.size();
    const auto usize = reader.uncompressed_size();
    const auto block_count = reader.block_count();
    const auto ratio = double(usize) / double(csize);

    std::cout << p << ":\n";
    std::cout << "block count: " << block_count << '\n';
    std::cout << "ratio: " << usize << "/" << csize << " → " << ratio << '\n';

    ratio_sum += ratio;
  }
  std::cout << "average ratio: " << ratio_sum / double(argc - 1) << '\n';
}
