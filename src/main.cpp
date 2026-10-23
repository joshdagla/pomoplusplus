#include "dtos/authDto.hpp"
#include "dtos/taskDto.hpp"
#include "services/authService.hpp"
#include "services/taskService.hpp"
#include <SQLiteCpp/Database.h>
#include <SQLiteCpp/Statement.h>
#include <bits/types/error_t.h>
#include <crow/app.h>
#include <crow/common.h>
#include <crow/exceptions.h>
#include <crow/http_request.h>
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
#include <vector>

int main() {
  crow::SimpleApp app;

  // TODO: Create blueprints for task CRUD routes.

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

        crow::json::wvalue res;

        if (!signUpResult.Success) {
          res["success"] = signUpResult.Success;
          res["message"] = signUpResult.ErrorMessage.value_or(
              "An unexpected error has occured while trying to register");

          return crow::response(401, res);
        }

        crow::json::wvalue jsonResult;
        jsonResult["message"] = "Login Successful";
        jsonResult["success"] = signUpResult.Success;
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

        crow::json::wvalue res;

        if (!loginResult.Success) {
          res["success"] = loginResult.Success;
          res["message"] = loginResult.ErrorMessage.value_or(
              "An unexpected error has occured while trying to login");

          return crow::response(401, res);
        }

        crow::json::wvalue jsonResult;
        jsonResult["message"] = "Login Successful";
        jsonResult["success"] = loginResult.Success;
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

  CROW_ROUTE(app, "/api/create/<int>")
      .methods(
          crow::HTTPMethod::POST)([](const crow::request &req, int userId) {
        ResultDto<std::string> result{};

        auto body = crow::json::load(req.body);

        CreateTaskDto newTask{};

        // Checks if request body and its values have been receieved or not
        if (!body || !body.has("description") || !body.has("pomodoros")) {

          result = createTaskService(newTask, userId);

          if (!result.Success) {
            if (result.ErrorMessage->empty()) {
              return crow::response(
                  401, "Bad Request. An unexpected error occured.");
            }

            return crow::response(
                401, result.ErrorMessage.value_or("Bad request error"));
          }

          crow::json::wvalue res;
          res["message"] = result.Data.value_or("Task created!");
          res["Success"] = result.Success;

          return crow::response(200, res);
        }

        newTask.Pomodoros = body["pomodoros"].i();
        newTask.Description = body["description"].s();

        result = createTaskService(newTask, userId);

        crow::json::wvalue res;

        if (!result.Success) {
          res["success"] = result.Success;
          res["message"] =
              result.ErrorMessage.value_or("An unexpected error has occured "
                                           "while trying to update the task");

          return crow::response(401, res);
        }

        res["message"] = result.Data.value_or("Task created!");
        res["Success"] = result.Success;

        return crow::response(200, res);
      });

  CROW_ROUTE(app, "/api/update/<int>")
      .methods(crow::HTTPMethod::PUT)([](const crow::request &req, int taskId) {
        auto body = crow::json::load(req.body);

        if (!body) {
          return crow::response(400, "Missing Parameters");
        }

        UpdateTaskDto updatedTask;

        if (body.has("description") &&
            body["description"].t() == crow::json::type::String) {
          updatedTask.Description = body["description"].s();
        }

        if (body.has("pomodoros") &&
            body["pomodoros"].t() == crow::json::type::Number) {
          updatedTask.Pomodoros = static_cast<int>(body["pomodoros"].i());
        }

        if (body.has("status") &&
            body["status"].t() == crow::json::type::String) {
          updatedTask.Status = body["status"].s();
        }

        if (!updatedTask.Description && !updatedTask.Pomodoros &&
            !updatedTask.Status) {
          return crow::response(400, "Nothing to update");
        }

        ResultDto<std::string> result = updateTaskService(updatedTask, taskId);

        crow::json::wvalue res;

        if (!result.Success) {
          res["success"] = false;
          res["message"] =
              result.ErrorMessage.value_or("An unexpected error has occurred "
                                           "while trying to update the task");

          return crow::response(401, res);
        }

        res["success"] = true;
        res["message"] = result.Data.value_or("Task updated successfully");

        return crow::response(200, res);
      });

  // Route to remove a task (not delete)
  CROW_ROUTE(app, "/api/remove/<int>")
      .methods(crow::HTTPMethod::PUT)([](int taskId) {
        ResultDto<std::string> result = removeTaskService(taskId);

        crow::json::wvalue res;

        if (!result.Success) {
          res["success"] = result.Success;
          res["message"] =
              result.ErrorMessage.value_or("An unexpected error has occured "
                                           "while trying to remove the task");

          return crow::response(401, res);
        }

        res["success"] = result.Success;
        res["message"] = result.Data.value_or("Task removed Successfully");

        return crow::response(200, res);
      });

  // Route to delete the task permanently
  CROW_ROUTE(app, "/api/delete/<int>")
      .methods(crow::HTTPMethod::DELETE)([](int taskId) {
        ResultDto<std::string> result = deleteTaskService(taskId);

        crow::json::wvalue res;

        if (!result.Success) {
          res["success"] = result.Success;
          res["message"] =
              result.ErrorMessage.value_or("An unexpected error has occured "
                                           "while trying to remove the task");

          return crow::response(401, res);
        }

        res["success"] = result.Success;
        res["message"] = result.Data.value_or("Task removed Successfully");

        return crow::response(200, res);
      });

  // Route to fetch all available tasks
  CROW_ROUTE(app, "/api/get/<int>")
      .methods(crow::HTTPMethod::GET)([](int userId) {
        FetchResultDto result = getTasks(userId);

        crow::json::wvalue res;

        if (!result.Success) {
          res["success"] = result.Success;
          res["message"] = result.ErrorMessage.value_or(
              "An unexpected error has occured while "
              "trying to fetch available tasks");

          return crow::response(401, res);
        }

        std::vector<crow::json::wvalue> tasksJson;
        for (const auto &task : result.Tasks) {
          crow::json::wvalue json;
          json["id"] = task.Id;
          json["description"] = task.Description;
          json["pomodoros"] = task.Pomodoros;
          json["status"] = task.Status;

          tasksJson.push_back(std::move(json));
        }

        res["success"] = result.Success;
        res["tasks"] = std::move(tasksJson);

        return crow::response(200, res);
      });

  // Route to fetch all tasks
  CROW_ROUTE(app, "/api/getAll/<int>")
      .methods(crow::HTTPMethod::GET)([](int userId) {
        FetchResultDto result = getAllTasks(userId);

        crow::json::wvalue res;

        if (!result.Success) {
          res["success"] = result.Success;
          res["message"] =
              result.ErrorMessage.value_or("An unexpected error has occured "
                                           "while trying to fetch all tasks");

          return crow::response(401, res);
        }

        std::vector<crow::json::wvalue> tasksJson;
        for (const auto &task : result.Tasks) {
          crow::json::wvalue json;
          json["id"] = task.Id;
          json["description"] = task.Description;
          json["pomodoros"] = task.Pomodoros;
          json["status"] = task.Status;

          tasksJson.push_back(std::move(json));
        }

        res["success"] = result.Success;
        res["message"] = std::move(tasksJson);

        return crow::response(200, res);
      });

  CROW_CATCHALL_ROUTE(app)([]() {
    auto page = crow::mustache::load("error.html");

    return page.render();
  });

  app.port(1808).multithreaded().run();
}
