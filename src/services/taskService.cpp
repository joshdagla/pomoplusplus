#include "taskService.hpp"
#include "../utils/userSearch.hpp"
#include "SQLiteCpp/Database.h"
#include <SQLiteCpp/Statement.h>
#include <SQLiteCpp/Transaction.h>
#include <crow/json.h>
#include <crow/routing.h>
#include <memory>
#include <optional>
#include <string>
#include <vector>

// Function to find user and return result of query
SearchResultDto findUser(const int &id) {
  std::unique_ptr<UserSearch> search = std::make_unique<UserSearch>(id);

  return search->UserSearchUtil();
}

// service to create a task
ResultDto<int> createTaskService(const CreateTaskDto &dto, int &userId) {
  try {

    ResultDto<int> serviceResponse;

    // checks if user exists and returns true if does exists, unless false
    SearchResultDto searchResult = findUser(userId);

    // return error statement and success false if user not found
    if (!searchResult.UserExists) {
      serviceResponse.Success = false;
      serviceResponse.ErrorMessage = "An error occured.";

      return serviceResponse;
    }

    SQLite::Database db("/data/pomo.db",
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

    int newId = static_cast<int>(db.getLastInsertRowid());

    serviceResponse.Success = true;
    serviceResponse.Data = newId;

    return serviceResponse;
  }

  catch (const SQLite::Exception &ex) {
    ResultDto<int> resultException;
    resultException.Success = false;
    std::string sqlErr = std::format("Database error. {}", ex.getErrorStr());
    resultException.ErrorMessage = sqlErr;

    return resultException;
  }

  catch (const std::exception &e) {
    ResultDto<int> resultException{};
    resultException.Success = false;
    resultException.ErrorMessage = e.what();

    return resultException;
  }
}

ResultDto<std::string> updateTaskService(const UpdateTaskDto &dto, int taskId) {
  try {
    ResultDto<std::string> serviceResponse;

    SQLite::Database db("/data/pomo.db", SQLite::OPEN_READWRITE);

    SQLite::Statement query(db, "UPDATE tasks SET "
                                "description = COALESCE(?, description), "
                                "pomodoros = COALESCE(?, pomodoros), "
                                "status = COALESCE(?, status) "
                                "WHERE id = ?");

    if (dto.Description.has_value()) {
      query.bind(1, dto.Description.value());
    } else {
      query.bind(1);
    }

    if (dto.Pomodoros.has_value()) {
      query.bind(2, dto.Pomodoros.value());
    } else {
      query.bind(2);
    }

    if (dto.Status.has_value()) {
      query.bind(3, dto.Status.value());
    } else {
      query.bind(3);
    }

    query.bind(4, taskId);

    int rowsChanged = query.exec();

    if (rowsChanged == 0) {
      serviceResponse.Success = false;
      serviceResponse.ErrorMessage = "Task not found";
      return serviceResponse;
    }

    serviceResponse.Success = true;
    serviceResponse.Data = "Task updated successfully";
    return serviceResponse;
  }

  catch (const SQLite::Exception &ex) {
    ResultDto<std::string> resultException;
    resultException.Success = false;
    resultException.ErrorMessage =
        std::format("Database error. {}", ex.getErrorStr());
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

    SQLite::Database db("/data/pomo.db", SQLite::OPEN_READWRITE);

    SQLite::Transaction transaction(db);

    SQLite::Statement removeQuery(db,
                                  "UPDATE tasks SET status = ? WHERE id = ?");

    removeQuery.bind(1, "Removed");
    removeQuery.bind(2, taskId);

    removeQuery.exec();

    transaction.commit();

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

    SQLite::Database db("/data/pomo.db", SQLite::OPEN_READWRITE);

    SQLite::Transaction transaction(db);

    SQLite::Statement removeQuery(db, "DELETE FROM tasks WHERE id = ?");

    removeQuery.bind(1, taskId);

    removeQuery.exec();

    transaction.commit();

    serviceResult.Success = true;
    serviceResult.Data = "Task deleted successfully";

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
FetchResultDto getTasks(int &userId) {

  try {
    FetchResultDto serviceResult;

    SearchResultDto searchResult = findUser(userId);

    // return error statement and success false if user not found
    if (!searchResult.UserExists) {
      serviceResult.Success = false;
      serviceResult.ErrorMessage = "An error occured.";

      return serviceResult;
    }

    SQLite::Database db("/data/pomo.db");

    SQLite::Statement getQuery(
        db, "SELECT * FROM tasks WHERE user_id = ? AND status != ?");

    getQuery.bind(1, userId);
    getQuery.bind(2, "Removed");

    std::vector<AllTasks> taskList;

    while (getQuery.executeStep()) {
      AllTasks a;

      a.Id = getQuery.getColumn("id").getInt();
      a.Description = getQuery.getColumn("description").getString();
      a.Pomodoros = getQuery.getColumn("pomodoros").getInt();
      a.Status = getQuery.getColumn("status").getString();

      taskList.push_back(a);
    }

    serviceResult.Success = true;
    serviceResult.Tasks = taskList;

    return serviceResult;
  }

  catch (const SQLite::Exception &ex) {
    FetchResultDto resultException;
    resultException.Success = false;
    std::string sqlErr = std::format("Database error. {}", ex.getErrorStr());
    resultException.ErrorMessage = sqlErr;

    return resultException;
  }

  catch (const std::exception &e) {
    FetchResultDto resultException;
    resultException.Success = false;
    resultException.ErrorMessage = e.what();

    return resultException;
  }
}

// Get every single task
FetchResultDto getAllTasks(int &userId) {

  try {
    FetchResultDto serviceResult;

    SearchResultDto searchResult = findUser(userId);

    // return error statement and success false if user not found
    if (!searchResult.UserExists) {
      serviceResult.Success = false;
      serviceResult.ErrorMessage = "An error occured.";

      return serviceResult;
    }

    SQLite::Database db("/data/pomo.db");

    SQLite::Statement getQuery(
        db, "SELECT * FROM tasks WHERE user_id = ? AND status = ?");

    getQuery.bind(1, userId);
    getQuery.bind(2, "Removed");

    std::vector<AllTasks> taskList;

    while (getQuery.executeStep()) {
      AllTasks a;

      a.Id = getQuery.getColumn("id").getInt();
      a.Description = getQuery.getColumn("description").getString();
      a.Pomodoros = getQuery.getColumn("pomodoros").getInt();
      a.Status = getQuery.getColumn("status").getString();

      taskList.push_back(a);
    }

    serviceResult.Success = true;
    serviceResult.Tasks = taskList;

    return serviceResult;
  }

  catch (const SQLite::Exception &ex) {
    FetchResultDto resultException;
    resultException.Success = false;
    std::string sqlErr = std::format("Database error. {}", ex.getErrorStr());
    resultException.ErrorMessage = sqlErr;

    return resultException;
  }

  catch (const std::exception &e) {
    FetchResultDto resultException{};
    resultException.Success = false;
    resultException.ErrorMessage = e.what();

    return resultException;
  }
}
