#include "dtos/authDto.hpp"
#include "services/authService.hpp"
#include <SQLiteCpp/Database.h>
#include <SQLiteCpp/Statement.h>
#include <crow/app.h>
#include <crow/common.h>
#include <crow/exceptions.h>
#include <crow/http_response.h>
#include <crow/json.h>
#include <crow/logging.h>
#include <crow/mustache.h>
#include <crow/returnable.h>
#include <crow/routing.h>
#include <iostream>
#include <optional>
#include <pwbcrypt/bcrypt.h>
#include <string>

int main() {
  crow::SimpleApp app;

  // Adjust app debugging level
  // app.loglevel(crow::LogLevel::WARNING);

  CROW_ROUTE(app, "/")([]() {
    auto page = crow::mustache::load("login.html");

    return page.render();
  });

  // User sign up endpoint
  CROW_ROUTE(app, "/auth/signup")
      .methods(crow::HTTPMethod::POST)([](const crow::request &req) {
        auto body = crow::json::load(req.body);

        if (!body) {
          std::cout << '\n' << "Missing username";
          return crow::response(400, "Missing parameters");
        }

        if (!body.has("email") || !body.has("password") ||
            !body.has("username")) {
          std::cout << '\n' << "Missing params";
          return crow::response(400, "Missing input field");
        }

        RegisterDto dto;
        dto.username = body["username"].s();
        dto.email = body["email"].s();
        dto.password = body["password"].s();

        if (dto.username.empty()) {
          std::cout << '\n' << "Missing params";
          return crow::response(400, "Missing username");
        }

        if (dto.email.empty() || dto.password.empty()) {
          std::cout << '\n' << "Missing params";
          return crow::response(400, "Missing email or password");
        }

        ResultDto signUpResult = userSignup(dto);

        if (!signUpResult.Success) {
          if (!signUpResult.ErrorMessage->empty()) {
            return crow::response(401, *signUpResult.ErrorMessage);
          }

          return crow::response(
              401, signUpResult.ErrorMessage.value_or("default value"));
        }

        crow::json::wvalue jsonResult;
        jsonResult["message"] = "Signup Successful";
        jsonResult["userId"] = signUpResult.Data.value_or(0);

        return crow::response(200, jsonResult);
      });

  CROW_ROUTE(app, "/auth/login")
      .methods(crow::HTTPMethod::POST)([](const crow::request &req) {
        auto userParams = crow::json::load(req.body);

        if (!userParams) {
          return crow::response(400, "Missing parameters");
        }

        if (!userParams.has("email") || !userParams.has("password")) {
          return crow::response(400, "Missing parameters");
        }

        LoginDto dto;
        dto.email = userParams["email"].s();
        dto.password = userParams["password"].s();

        ResultDto loginResult = validateLogin(dto);

        if (!loginResult.Success) {
          if (!loginResult.ErrorMessage->empty()) {
            return crow::response(401, *loginResult.ErrorMessage);
          }

          return crow::response(
              401, loginResult.ErrorMessage.value_or("default value"));
        }

        crow::json::wvalue jsonResult;
        jsonResult["message"] = "Login Successful";
        jsonResult["userId"] = loginResult.Data.value_or(0);

        return crow::response(200, jsonResult);
      });

  CROW_ROUTE(app, "/home/<int>")([](int userId) {
    if (userId == 0) {
      crow::response res;
      res.redirect("/error");
      return res;
    }

    SQLite::Database db("pomo.db", SQLite::OPEN_READONLY);

    SQLite::Statement usernameFetch(db,
                                    "SELECT username from users WHERE id = ?");

    usernameFetch.bind(1, userId);

    std::string userString;

    if (usernameFetch.executeStep()) {
      userString = usernameFetch.getColumn(0).getText();
    } else {
      crow::response res;
      res.redirect("/error");
      return res;
    }

    crow::mustache::context ctx;
    ctx["name"] = userString;

    auto page = crow::mustache::load("index.html");

    return crow::response(page.render(ctx));
  });

  CROW_CATCHALL_ROUTE(app)([]() {
    auto page = crow::mustache::load("error.html");

    return page.render();
  });

  app.port(1808).multithreaded().run();
}
