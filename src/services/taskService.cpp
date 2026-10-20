#include "taskService.hpp"
#include "SQLiteCpp/Database.h"
#include <vector>

// service to create a task
ResultDto createTask(CreateTaskDto &dto) {
    SQLite::Database db ("pomo.db", SQLite::OPEN_CREATE | SQLite::OPEN_READWRITE);

    ResultDto serviceResponse;

    std::vector<int> woe {1,2};
}
