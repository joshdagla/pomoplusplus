#include "userSearch.hpp"
#include <SQLiteCpp/Database.h>
#include <SQLiteCpp/Statement.h>

// Class contructor
UserSearch::UserSearch(int u) : m_userId(u) {};

// User search method
SearchResultDto UserSearch::UserSearchUtil() {
  SearchResultDto result;

  SQLite::Database db("/data/pomo.db", SQLite::OPEN_READONLY);

  SQLite::Statement userSearchQuery(db, "SELECT id FROM users where id = ?");

  userSearchQuery.bind(1, m_userId);

  if (userSearchQuery.executeStep()) {
    result.UserExists = true;
    result.UserId = userSearchQuery.getColumn(0);
  } else {
    result.UserExists = false;
    result.UserId = 0;
  }

  return result;
}
