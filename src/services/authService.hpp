#pragma once
#include "../dtos/authDto.hpp"
#include <string>

bool validateEmail(const std::string &email);
ResultDto<int> userSignup(const RegisterDto &dto);
ResultDto<int> validateLogin(const LoginDto &dto);
