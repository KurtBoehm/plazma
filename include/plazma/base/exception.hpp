#ifndef INCLUDE_PLAZMA_BASE_EXCEPTION_HPP
#define INCLUDE_PLAZMA_BASE_EXCEPTION_HPP

#include <exception>
#include <sstream>
#include <string>
#include <utility>

namespace plazma {
struct Exception : public std::exception {
  explicit Exception(std::string msg) : message_(std::move(msg)) {}
  template<typename... TArgs>
  explicit Exception(TArgs&&... args)
      : message_((std::stringstream{} << ... << std::forward<TArgs>(args)).str()) {}

  [[nodiscard]] const char* what() const noexcept override {
    return message_.c_str();
  }

private:
  std::string message_;
};
} // namespace plazma

#endif // INCLUDE_PLAZMA_BASE_EXCEPTION_HPP
