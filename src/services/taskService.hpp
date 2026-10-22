#pragma once
#include "../dtos/authDto.hpp"
#include "../dtos/taskDto.hpp"
#include "../utils/userSearch.hpp"

SearchResultDto findUser(const int &id);

ResultDto<std::string> createTaskService(const CreateTaskDto &dto, int &userId);

ResultDto<std::string> updateTaskService(UpdateTaskDto &dto, int &taskId);

ResultDto<std::string> removeTaskService(int &taskId);

ResultDto<std::string> deleteTaskService(int &taskId);

// Gets all tasks that arent removed
ResultDto<std::string> getTasks(int &userId);

// Gets every task, even removed ones
ResultDto<std::string> getAllTasks(int &userId);
