#pragma once
#include "../dtos/authDto.hpp"
#include "../dtos/taskDto.hpp"
#include "../utils/userSearch.hpp"
#include <string>
#include <vector>

// Vector to hold values of user tasks
struct AllTasks {
  short Id;
  std::string Description;
  short Pomodoros;
  std::string Status;
};

SearchResultDto findUser(const int &id);

ResultDto<std::string> createTaskService(const CreateTaskDto &dto, int &userId);

ResultDto<std::string> updateTaskService(UpdateTaskDto &dto, int &taskId);

ResultDto<std::string> removeTaskService(int &taskId);

ResultDto<std::string> deleteTaskService(int &taskId);

// Gets all tasks that arent removed
ResultDto<std::vector<AllTasks>> getTasks(int &userId);

// Gets every task, even removed ones
ResultDto<std::vector<AllTasks>> getAllTasks(int &userId);
