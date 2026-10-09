cmake_minimum_required(VERSION 3.15)
project(PomoPlusPlus)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Crow REQUIRED)
find_package(asio REQUIRED)
find_package(SQLiteCpp REQUIRED)

message(STATUS "Building with CMake version: ${CMAKE_VERSION}")

add_executable(${PROJECT_NAME} 
  src/main.cpp
  src/dtos/authDto.hpp
  src/services/authService.cpp
)

if(WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        _WIN32_WINNT=0x0A00
        NTDDI_VERSION=0x0A000000
    )
endif()

target_include_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/lib
)

target_link_libraries(${PROJECT_NAME} PRIVATE Crow::Crow asio::asio SQLiteCpp pwbcrypt bcrypt ws2_32 mswsock)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/templates
            $<TARGET_FILE_DIR:${PROJECT_NAME}>/templates
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/public
            $<TARGET_FILE_DIR:${PROJECT_NAME}>/public
)
