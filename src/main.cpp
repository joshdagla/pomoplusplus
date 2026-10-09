#include "dtos/authDto.hpp"
#include "services/authService.hpp"
#include <crow/app.h>
#include <crow/common.h>
#include <crow/exceptions.h>
#include <crow/http_response.h>
#include <crow/json.h>
#include <crow/mustache.h>
#include <crow/returnable.h>
#include <crow/routing.h>
#include <crow/mustache.h>
#include <pwbcrypt/bcrypt.h>
#include <string>

int main() {
  crow::SimpleApp app;

  crow::mustache::set_global_base("templates");

  // Login page rendering endpoint
  CROW_ROUTE(app, "/")([]() {
    auto page = crow::mustache::load("login.html");
    return page.render();
  });

  // Endpoint to server static files such as images and fonts
  CROW_ROUTE(app, "/public/<path>")
  ([](const crow::request &req, crow::response &res, std::string path) {
    res.set_static_file_info("public/" + path);
    res.end();
  });

  // User sign up endpoint
  CROW_ROUTE(app, "/signup")
      .methods(crow::HTTPMethod::POST)([](const crow::request &req) {
        auto userParams = req.get_body_params();

        const char *username = userParams.get("username");
        const char *email = userParams.get("email");
        const char *password = userParams.get("password");

        if (!username) {
          return crow::response(400, "Missing parameters");
        }

        if (!email || !password) {
          return crow::response(400, "Missing parameters");
        }

        RegisterDto dto;
        dto.username = username;
        dto.email = email;
        dto.password = password;

        if (dto.username.empty()) {
          return crow::response(400, "Missing parameters");
        }

        if (dto.email.empty() || dto.password.empty()) {
          return crow::response(400, "Missing parameters");
        }

        ResultDto signUpResult = userSignup(dto);

        if (!signUpResult.Success) {
          if (!signUpResult.ErrorMessage->empty()) {
            return crow::response(401, *signUpResult.ErrorMessage);
          }

          return crow::response(
              401, signUpResult.ErrorMessage.value_or("default value"));
        }

        return crow::response(200, "User created successfully");
      });

  CROW_ROUTE(app, "/login")
      .methods(crow::HTTPMethod::POST)([](const crow::request &req) {
        auto userParams = req.get_body_params();

        const char *email = userParams.get("email");
        const char *password = userParams.get("password");

        if (!email || !password) {
          return crow::response(400, "Missing parameters");
        }

        LoginDto dto;
        dto.email = email;
        dto.password = password;

        if (dto.email.empty() || dto.password.empty()) {
          return crow::response(400, "Missing parameters");
        }

        ResultDto loginResult = validateLogin(dto);

        if (!loginResult.Success) {
          if (!loginResult.ErrorMessage->empty()) {
            return crow::response(401, *loginResult.ErrorMessage);
          }

          return crow::response(
              401, loginResult.ErrorMessage.value_or("default value"));
        }

        return crow::response(200, "Login successful");
      });

  CROW_CATCHALL_ROUTE(app)([]() {
    auto errorPage = crow::mustache::load("error.html");
    return errorPage.render();
  });

  app.port(1808).multithreaded().run();
}
