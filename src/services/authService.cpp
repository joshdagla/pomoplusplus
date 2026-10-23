#include "authService.hpp"
#include "../dtos/authDto.hpp"
#include "pwbcrypt/bcrypt.h"
#include <SQLiteCpp/Database.h>
#include <SQLiteCpp/Exception.h>
#include <SQLiteCpp/SQLiteCpp.h>
#include <SQLiteCpp/Statement.h>
#include <SQLiteCpp/Transaction.h>
#include <crow/app.h>
#include <crow/logging.h>
#include <exception>
#include <optional>
#include <regex>
#include <string>

// Function to validate emails
bool validateEmail(const std::string &email) {
  const std::regex pattern(
      R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)");

  return std::regex_match(email, pattern);
}

// Handler to do user signup
ResultDto<int> userSignup(const RegisterDto &dto) {

  try {

    SQLite::Database db("pomo.db",
                        SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);

    ResultDto<int> response;

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

    SQLite::Statement userIdQuery(db, "SELECT id from users where email = ?");

    userIdQuery.bind(1, dto.email);

    int userId;

    if (userIdQuery.executeStep()) {
      userId = userIdQuery.getColumn(0).getInt();
    }

    response.Success = true;
    response.Data = userId;

    CROW_LOG_DEBUG << "Debugging... UserId = " << response.Data.value_or(0);

    return response;
  }

  catch (const SQLite::Exception &ex) {
    ResultDto<int> resultException;
    resultException.Success = false;
    std::string sqlErr = std::format("Database error. {}", ex.getErrorStr());
    resultException.ErrorMessage = sqlErr;
    resultException.Data = std::nullopt;

    return resultException;
  }

  catch (const std::exception &e) {

    ResultDto<int> resultException{};
    resultException.Success = false;
    resultException.ErrorMessage = e.what();
    resultException.Data = std::nullopt;

    return resultException;
  }
}

// Handler for user login
ResultDto<int> validateLogin(const LoginDto &dto) {
  try {
    SQLite::Database db("pomo.db", SQLite::OPEN_READONLY);

    ResultDto<int> response;

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

    SQLite::Statement emailQuery(db,
                                 "SELECT password FROM users WHERE email = ?");
    emailQuery.bind(1, dto.email);

    if (!emailQuery.executeStep()) {
      response.Success = false;
      response.ErrorMessage = "Invalid email or password";
      return response;
    }

    std::string passwordHash = emailQuery.getColumn("password").getString();

    if (!bcrypt::validatePassword(dto.password, passwordHash)) {
      response.Success = false;
      response.ErrorMessage = "Invalid email or password";
      return response;
    }

    SQLite::Statement userIdQuery(db, "SELECT id from users where email = ?");

    userIdQuery.bind(1, dto.email);

    int userId;

    if (userIdQuery.executeStep()) {
      userId = userIdQuery.getColumn(0).getInt();
    }

    response.Success = true;
    response.Data = userId;

    CROW_LOG_DEBUG << "Debugging... UserId = " << response.Data.value_or(0);

    return response;
  }

  catch (const SQLite::Exception &ex) {
    ResultDto<int> resultException;
    resultException.Success = false;
    std::string sqlErr = std::format("Database error. {}", ex.getErrorStr());
    resultException.ErrorMessage = sqlErr;
    resultException.Data = std::nullopt;

    return resultException;
  }

  catch (const std::exception &e) {
    ResultDto<int> resultException{};
    resultException.Success = false;
    resultException.ErrorMessage = e.what();
    resultException.Data = std::nullopt;

    return resultException;
  }
}
