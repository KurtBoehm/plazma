// This file is part of https://github.com/KurtBoehm/plazma.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef INCLUDE_PLAZMA_BASE_EXCEPTION_HPP
#define INCLUDE_PLAZMA_BASE_EXCEPTION_HPP

#include <exception>
#include <string>
#include <utility>

namespace plazma {
struct Exception : public std::exception {
  explicit Exception(std::string msg) : message_(std::move(msg)) {}

  [[nodiscard]] const char* what() const noexcept override {
    return message_.c_str();
  }

private:
  std::string message_;
};
} // namespace plazma

#endif // INCLUDE_PLAZMA_BASE_EXCEPTION_HPP
