#pragma once
#include <SQLiteCpp/Database.h>
#include <SQLiteCpp/Statement.h>
#include <optional>

// Returns
struct SearchResultDto {
  std::optional<int> UserId = 0;
  bool UserExists;
};

class UserSearch {

private:
  int m_userId;

public:
  UserSearch(int userId); // constructor

  SearchResultDto
  UserSearchUtil(); // static method that checks if user exists or not
};
