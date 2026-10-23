#pragma once
#include <optional>
#include <string>

struct CreateTaskDto {
  short UserId;
  std::optional<std::string> Description = "New Task";
  short Pomodoros = 0;
};

struct UpdateTaskDto {
  short UserId;
  std::optional<std::string> Description;
  std::optional<int> Pomodoros;
  std::optional<std::string> Status = "Pending";
};
