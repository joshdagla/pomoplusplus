#pragma once
#include "../dtos/authDto.hpp"
#include <string>

bool validateEmail(const std::string &email);
ResultDto userSignup(const RegisterDto &dto);
ResultDto validateLogin(const LoginDto &dto);
