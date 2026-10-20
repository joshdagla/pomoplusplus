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
  src/services/authService.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/lib
)
# Keep Conan-managed dependencies
target_link_libraries(${PROJECT_NAME} PRIVATE Crow::Crow asio::asio SQLiteCpp bcrypt)
