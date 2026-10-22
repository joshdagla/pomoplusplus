#pragma once
#include <optional>
#include <string>

struct RegisterDto {
  std::string username;
  std::string email;
  std::string password;
};

struct LoginDto {
  std::string email;
  std::string password;
};

// Generic template for returning Data on service
template <typename T>

struct ResultDto {
  bool Success;
  std::optional<std::string> ErrorMessage = "An unexpected error has occured";
  std::optional<T> Data;
};
