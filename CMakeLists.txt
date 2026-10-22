cmake_minimum_required(VERSION 3.15)
project(PomoPlusPlus)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

message(STATUS "Building with CMake version: ${CMAKE_VERSION}")

find_package(Crow REQUIRED)
find_package(asio REQUIRED)
find_package(SQLiteCpp REQUIRED)

add_executable(${PROJECT_NAME}
  src/main.cpp
  src/dtos/authDto.hpp
  src/dtos/taskDto.hpp
  src/services/authService.cpp
  src/services/taskService.cpp
  src/utils/userSearch.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/lib
)
# Keep Conan-managed dependencies
target_link_libraries(${PROJECT_NAME} PRIVATE Crow::Crow asio::asio SQLiteCpp
                      bcrypt)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND}
                   -E create_symlink ${CMAKE_SOURCE_DIR}/templates
                   $<TARGET_FILE_DIR:${PROJECT_NAME}>/templates COMMAND
                   ${CMAKE_COMMAND} -E create_symlink
                   ${CMAKE_SOURCE_DIR}/public
                   $<TARGET_FILE_DIR:${PROJECT_NAME}>/static)
