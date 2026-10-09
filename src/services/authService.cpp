#include "authService.hpp"
#include "../dtos/authDto.hpp"
#include "pwbcrypt/bcrypt.h"
#include <SQLiteCpp/Database.h>
#include <SQLiteCpp/Exception.h>
#include <SQLiteCpp/SQLiteCpp.h>
#include <SQLiteCpp/Statement.h>
#include <SQLiteCpp/Transaction.h>
#include <crow/app.h>
#include <exception>
#include <processthreadsapi.h>
#include <regex>
#include <string>

// Function to validate emails
bool validateEmail(const std::string &email) {
  const std::regex pattern(
      R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)");

  return std::regex_match(email, pattern);
}

// Handler to do user signup
ResultDto userSignup(const RegisterDto &dto) {

  try {

    SQLite::Database db("pomo.db",
                        SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);

    ResultDto response;

    if (dto.username.empty()) {
      response.Success = false;
      response.ErrorMessage = "Username cannot be empty";

      return response;
    }

    if (dto.email.empty() || dto.password.empty()) {
      response.Success = false;
      response.ErrorMessage = "Email or password fields cannot be empty";

      return response;
    }

    if (!validateEmail(dto.email)) {
      response.Success = false;
      response.ErrorMessage = "Email is invalid";

      return response;
    }

    std::string passwordHash = bcrypt::generateHash(dto.password);

    SQLite::Statement emailQuery(db, "SELECT * FROM users WHERE email = ?");

    emailQuery.bind(1, dto.email);

    bool queryResult = emailQuery.executeStep();

    if (queryResult == true) {
      response.Success = false;
      response.ErrorMessage = "User with same email already exists";

      return response;
    }

    SQLite::Statement usernameQuery(
        db, "SELECT username FROM users WHERE username = ?");

    usernameQuery.bind(1, dto.username);

    bool nameQueryResult = usernameQuery.executeStep();

    if (nameQueryResult) {
      response.Success = false;
      response.ErrorMessage = "Username already exists";

      return response;
    }

    SQLite::Transaction transaction(db);

    SQLite::Statement insertQuery(
        db, "INSERT INTO users (username, email, password) VALUES (?, ?, ?)");

    insertQuery.bind(1, dto.username);
    insertQuery.bind(2, dto.email);
    insertQuery.bind(3, passwordHash);

    insertQuery.exec();

    transaction.commit();

    response.Success = true;

    return response;
  }

  catch (const SQLite::Exception &ex) {
      ResultDto resultException;
      resultException.Success = false;
      resultException.ErrorMessage = ex.what();

      return resultException;
  }

  catch (const std::exception &e) {

    ResultDto resultException{};
    resultException.Success = false;
    resultException.ErrorMessage = e.what();

    return resultException;
  }
}

// Handler for user login
ResultDto validateLogin(const LoginDto &dto) {
  try {
    SQLite::Database db("pomo.db", SQLite::OPEN_READONLY);

    ResultDto response;

    if (dto.email.empty() || dto.password.empty()) {
      response.Success = false;
      response.ErrorMessage = "Email or password fields cannot be empty";

      return response;
    }

    if (!validateEmail(dto.email)) {
      response.Success = false;
      response.ErrorMessage = "Email is invalid";

      return response;
    }

    SQLite::Statement emailQuery(db, "SELECT * FROM users WHERE email = ?");

    emailQuery.bind(1, dto.email);

    bool queryResult = emailQuery.executeStep();

    if (queryResult == false) {
      response.Success = false, response.ErrorMessage = "Email does not exist";

      return response;
    }

    bool verifyPassword = false;

    while (emailQuery.executeStep()) {
      auto passwordHash = emailQuery.getColumn("password");

      verifyPassword = bcrypt::validatePassword(dto.password, passwordHash);
    }

    if (verifyPassword == false) {
      response.Success = false;
      response.ErrorMessage = "Incorrect Password";

      return response;
    }

    response.Success = true;

    return response;


  }

  catch (const SQLite::Exception &ex) {
      ResultDto resultException;
      resultException.Success = false;
      resultException.ErrorMessage = ex.what();

      return resultException;
  }

  catch (const std::exception &e) {
    ResultDto resultException{};
    resultException.Success = false;
    resultException.ErrorMessage = e.what();

    return resultException;
  }
}
