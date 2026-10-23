#pragma once
#include "../dtos/authDto.hpp"
#include "../dtos/taskDto.hpp"
#include "../utils/userSearch.hpp"
#include <optional>
#include <string>
#include <vector>

// Vector to hold values of user tasks
struct AllTasks {
  short Id;
  std::string Description;
  short Pomodoros;
  std::string Status;
};

struct FetchResultDto {
  bool Success;
  std::optional<std::string> ErrorMessage;
  std::vector<AllTasks> Tasks;
};

SearchResultDto findUser(const int &id);

ResultDto<std::string> createTaskService(const CreateTaskDto &dto, int &userId);

ResultDto<std::string> updateTaskService(const UpdateTaskDto &dto, int taskId);

ResultDto<std::string> removeTaskService(int &taskId);

ResultDto<std::string> deleteTaskService(int &taskId);

// Gets all tasks that arent removed
FetchResultDto getTasks(int &userId);

// Gets every task, even removed ones
FetchResultDto getAllTasks(int &userId);
