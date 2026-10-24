#pragma once
#include <chrono>
#include <crow/app.h>
#include <fstream>
#include <jwt-cpp/traits/kazuho-picojson/defaults.h>
#include <optional>
#include <string>

inline const std::string &jwtSecret() {
  static const std::string secret = [] {
    std::ifstream file("config.json");
    if (!file) {
      throw std::runtime_error("config.json not found");
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    auto cfg = crow::json::load(buffer.str());
    if (!cfg || !cfg.has("jwtSecret")) {
      throw std::runtime_error("config.json is missing jwt_secret");
    }

    return std::string(cfg["jwtSecret"].s());
  }();

  return secret;
}

inline std::string createToken(int userId) {
  auto now = std::chrono::system_clock::now();
  return jwt::create()
      .set_type("JWT")
      .set_issuer("pomoplusplus")
      .set_subject(std::to_string(userId))
      .set_issued_at(now)
      .set_expires_at(now + std::chrono::hours(24))
      .sign(jwt::algorithm::hs256{jwtSecret()});
}

// Returns the user id if the token is valid, otherwise nothing
inline std::optional<int> verifyToken(const std::string &token) {
  try {
    auto decoded = jwt::decode(token);
    jwt::verify()
        .allow_algorithm(jwt::algorithm::hs256{jwtSecret()})
        .with_issuer("pomoplusplus")
        .verify(decoded); // throws if signature is bad or token expired
    return std::stoi(decoded.get_subject());
  } catch (...) {
    return std::nullopt;
  }
}
