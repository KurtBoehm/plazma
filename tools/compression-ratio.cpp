#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "argparse/argparse.hpp"
#include "plazma/plazma.hpp"

struct Args {
  std::vector<std::filesystem::path> paths;
};

Args parse_args(int argc, const char** argv) {
  argparse::ArgumentParser parser("sizes");
  parser.add_argument("files").nargs(argparse::nargs_pattern::at_least_one);
  try {
    parser.parse_args(argc, argv);
    const auto files_str = parser.get<std::vector<std::string>>("files");

    std::vector<std::filesystem::path> files{};
    files.reserve(files_str.size());
    for (const auto& file_str : files_str) {
      files.push_back(std::filesystem::canonical(file_str));
    }

    return {files};
  } catch (const std::runtime_error& err) {
    std::cerr << err.what() << '\n';
    std::cerr << parser;
    std::exit(1);
  }
}

int main(int argc, const char** argv) {
  Args args = parse_args(argc, argv);
  double ratio_sum = 0;
  for (const auto& p : args.paths) {
    if (!std::filesystem::exists(p)) {
      std::cerr << p << " does not exist!\n";
    }

    plazma::Reader reader(p);
    const auto csize = reader.size();
    const auto usize = reader.uncompressed_size();
    const auto ratio = double(usize) / double(csize);

    std::cout << p << " ratio: " << usize << "/" << csize << " → " << ratio << '\n';
    ratio_sum += ratio;
  }
  std::cout << "average ratio: " << ratio_sum / double(args.paths.size()) << '\n';
}
