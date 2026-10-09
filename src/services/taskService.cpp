#include "taskService.hpp"
#include "SQLiteCpp/Database.h"

// service to create a task
ResultDto createTask(CreateTaskDto &dto) {
    SQLite::Database db ("pomo.db", SQLite::OPEN_CREATE | SQLite::OPEN_READWRITE);

    ResultDto serviceResponse;


}
