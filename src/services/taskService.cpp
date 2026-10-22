#include "taskService.hpp"
#include "../utils/userSearch.hpp"
#include "SQLiteCpp/Database.h"
#include <SQLiteCpp/Statement.h>
#include <SQLiteCpp/Transaction.h>
#include <crow/json.h>
#include <crow/routing.h>
#include <format>
#include <memory>
#include <optional>
#include <string>

// Function to find user and return result of query
SearchResultDto findUser(const int &id) {
  std::unique_ptr<UserSearch> search = std::make_unique<UserSearch>(id);

  return search->UserSearchUtil();
}

// service to create a task
ResultDto<std::string> createTaskService(const CreateTaskDto &dto,
                                         int &userId) {
  try {

    ResultDto<std::string> serviceResponse;

    // checks if user exists and returns true if does exists, unless false
    SearchResultDto searchResult = findUser(userId);

    // return error statement and success false if user not found
    if (!searchResult.UserExists) {
      serviceResponse.Success = false;
      serviceResponse.ErrorMessage = "An error occured.";

      return serviceResponse;
    }

    SQLite::Database db("pomo.db",
                        SQLite::OPEN_CREATE | SQLite::OPEN_READWRITE);

    SQLite::Transaction transaction(db);

    // create new task
    SQLite::Statement newTaskQuery(db,
                                   "INSERT INTO tasks (description, pomodoros, "
                                   "status, user_id) VALUES (?, ?, ?, ?)");

    newTaskQuery.bind(1, dto.Description.value_or("New Task"));
    newTaskQuery.bind(2, dto.Pomodoros);
    newTaskQuery.bind(3, "Pending");
    newTaskQuery.bind(4, userId);

    newTaskQuery.exec();

    transaction.commit();

    serviceResponse.Success = true;
    serviceResponse.Data = "Task created successfully";

    return serviceResponse;
  }

  catch (const SQLite::Exception &ex) {
    ResultDto<std::string> resultException;
    resultException.Success = false;
    std::string sqlErr = std::format("Database error. {}", ex.getErrorStr());
    resultException.ErrorMessage = sqlErr;

    return resultException;
  }

  catch (const std::exception &e) {
    ResultDto<std::string> resultException{};
    resultException.Success = false;
    resultException.ErrorMessage = e.what();

    return resultException;
  }
}

ResultDto<std::string> UpdateTaskDto(UpdateTaskDto &dto, int &taskId) {
  try {
    ResultDto<std::string> serviceResponse;

    SQLite::Database db("pomo.db");

    SQLite::Transaction transaction(db);

    // If description and pomodoros are not provided, only status will be
    // updated
    if (!dto.Description.has_value() || !dto.Pomodoros.has_value()) {
      SQLite::Statement updateTaskQuery(
          db, "UPDATE tasks (status) SET VALUES (?) WHERE id = ?");

      updateTaskQuery.bind(1, dto.Status);
      updateTaskQuery.bind(2, taskId);

      updateTaskQuery.exec();

      transaction.commit();

      serviceResponse.Success = true;
      serviceResponse.Data = "Task updated successfully";

      return serviceResponse;
    }
    // If only description is not provided
    else if (!dto.Description.has_value()) {
      SQLite::Statement updateTaskQuery(
          db,
          "UPDATE tasks (pomodoros, status) SET VALUES (?, ?) WHERE id = ?");

      updateTaskQuery.bind(1, dto.Pomodoros.value_or(0));
      updateTaskQuery.bind(2, dto.Status);
      updateTaskQuery.bind(3, taskId);

      updateTaskQuery.exec();

      transaction.commit();

      serviceResponse.Success = true;
      serviceResponse.Data = "Task updated successfully";

      return serviceResponse;
    }
    // If only pomodoros arent provided
    else if (!dto.Pomodoros.has_value()) {
      SQLite::Statement updateTaskQuery(
          db,
          "UPDATE tasks (description, status) SET VALUES (?, ?) WHERE id = ?");

      updateTaskQuery.bind(1, dto.Description.value_or("Updated Task"));
      updateTaskQuery.bind(2, dto.Status);
      updateTaskQuery.bind(3, taskId);

      updateTaskQuery.exec();

      transaction.commit();

      serviceResponse.Success = true;
      serviceResponse.Data = "Task updated successfully";

      return serviceResponse;
    }

    // If all parameters are provided
    SQLite::Statement updateTaskQuery(
        db, "UPDATE tasks (description, pomodoros, status) SET VALUES (?, ?, "
            "?) WHERE id = ?");

    updateTaskQuery.bind(1, dto.Description.value_or("Updated Task"));
    updateTaskQuery.bind(2, dto.Pomodoros.value_or(0));
    updateTaskQuery.bind(3, dto.Status);
    updateTaskQuery.bind(4, taskId);

    updateTaskQuery.exec();

    transaction.commit();

    serviceResponse.Success = true;
    serviceResponse.Data = "Task updated successfully";

    return serviceResponse;
  }

  catch (const SQLite::Exception &ex) {
    ResultDto<std::string> resultException;
    resultException.Success = false;
    std::string sqlErr = std::format("Database error. {}", ex.getErrorStr());
    resultException.ErrorMessage = sqlErr;

    return resultException;
  }

  catch (const std::exception &e) {
    ResultDto<std::string> resultException{};
    resultException.Success = false;
    resultException.ErrorMessage = e.what();

    return resultException;
  }
}

// Sets task status to deleted
ResultDto<std::string> removeTaskService(int &taskId) {
  try {
    ResultDto<std::string> serviceResult;

    SQLite::Database db("pomo.db");

    SQLite::Transaction transaction(db);

    SQLite::Statement removeQuery(
        db, "UPDATE tasks (status) SET VALUES (?) WHERE id = ?");

    removeQuery.bind(1, "Removed");
    removeQuery.bind(2, taskId);

    serviceResult.Success = true;
    serviceResult.Data = "Task removed. Still available in Tasks section";

    return serviceResult;
  }

  catch (const SQLite::Exception &ex) {
    ResultDto<std::string> resultException;
    resultException.Success = false;
    std::string sqlErr = std::format("Database error. {}", ex.getErrorStr());
    resultException.ErrorMessage = sqlErr;

    return resultException;
  }

  catch (const std::exception &e) {
    ResultDto<std::string> resultException{};
    resultException.Success = false;
    resultException.ErrorMessage = e.what();

    return resultException;
  }
}

ResultDto<std::string> deleteTaskService(int &taskId) {

  try {
    ResultDto<std::string> serviceResult;

    SQLite::Database db("pomo.db");

    SQLite::Transaction transaction(db);

    SQLite::Statement removeQuery(db, "DELETE FROM tasks WHERE id = ?");

    removeQuery.bind(1, taskId);

    serviceResult.Success = true;
    serviceResult.Data = "Task removed. Still available in Tasks section";

    return serviceResult;
  }

  catch (const SQLite::Exception &ex) {
    ResultDto<std::string> resultException;
    resultException.Success = false;
    std::string sqlErr = std::format("Database error. {}", ex.getErrorStr());
    resultException.ErrorMessage = sqlErr;

    return resultException;
  }

  catch (const std::exception &e) {
    ResultDto<std::string> resultException{};
    resultException.Success = false;
    resultException.ErrorMessage = e.what();

    return resultException;
  }
}

// Get tasks not yet removed
ResultDto<std::string> getTasks(int &userId) {

  try {
    ResultDto<std::string> serviceResult;

    SearchResultDto searchResult = findUser(userId);

    // return error statement and success false if user not found
    if (!searchResult.UserExists) {
      serviceResult.Success = false;
      serviceResult.ErrorMessage = "An error occured.";

      return serviceResult;
    }

    SQLite::Database db("pomo.db");

    SQLite::Transaction transaction(db);

    SQLite::Statement getQuery(
        db, "SELECT * FROM tasks WHERE user_id = ? AND status != removed");

    getQuery.bind(1, userId);

    // TODO: Put received values thru get column into a list
    while (getQuery.executeStep()) {
    }

    serviceResult.Success = true;
    serviceResult.Data = "Tasks fetched successfully";

    return serviceResult;
  }

  catch (const SQLite::Exception &ex) {
    ResultDto<std::string> resultException;
    resultException.Success = false;
    std::string sqlErr = std::format("Database error. {}", ex.getErrorStr());
    resultException.ErrorMessage = sqlErr;

    return resultException;
  }

  catch (const std::exception &e) {
    ResultDto<std::string> resultException{};
    resultException.Success = false;
    resultException.ErrorMessage = e.what();

    return resultException;
  }
}

// Get every single task
ResultDto<std::string> getAllTasks(int &userId) {

  try {
    ResultDto<std::string> serviceResult;

    SearchResultDto searchResult = findUser(userId);

    // return error statement and success false if user not found
    if (!searchResult.UserExists) {
      serviceResult.Success = false;
      serviceResult.ErrorMessage = "An error occured.";

      return serviceResult;
    }

    SQLite::Database db("pomo.db");

    SQLite::Transaction transaction(db);

    SQLite::Statement getQuery(db, "SELECT * FROM tasks WHERE user_id = ?");

    getQuery.bind(1, userId);

    // TODO: Put received values thru get column into a list
    while (getQuery.executeStep()) {
    }

    serviceResult.Success = true;
    serviceResult.Data = "Tasks fetched successfully";

    return serviceResult;
  }

  catch (const SQLite::Exception &ex) {
    ResultDto<std::string> resultException;
    resultException.Success = false;
    std::string sqlErr = std::format("Database error. {}", ex.getErrorStr());
    resultException.ErrorMessage = sqlErr;

    return resultException;
  }

  catch (const std::exception &e) {
    ResultDto<std::string> resultException{};
    resultException.Success = false;
    resultException.ErrorMessage = e.what();

    return resultException;
  }
}
