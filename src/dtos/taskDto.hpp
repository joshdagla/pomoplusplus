#pragma once
#include <optional>
#include <string>

struct CreateTaskDto {
    int UserId;
    std::string Description = "New Task";
    int Pomodoros = 0;
};

struct UpdateTaskDto {
    int UserId;
    std::optional<std::string> Description;
    std::optional<std::string> Pomodoros;
    std::optional<std::string> Status;
};

struct DeleteTaskDto {
    int UserId;
    int TaskId;
};
