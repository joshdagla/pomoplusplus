#pragma once
#include "../dtos/taskDto.hpp"
#include "../dtos/authDto.hpp"

ResultDto createTask(CreateTaskDto &dto);

ResultDto updateTask(UpdateTaskDto &dto);

ResultDto deleteTask(DeleteTaskDto &dto);
