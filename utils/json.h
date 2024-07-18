#pragma once

#include <memory>
#include <optional>
#include <vector>
#include <string_view>
#include <string>

struct JsonImpl;

struct Json {
  std::unique_ptr<JsonImpl> impl;
  Json();
  Json(std::string_view content);
  ~Json();
  Json(const Json &rhs);
  void operator =(const Json &rhs);
  std::optional<int> getInt(std::string_view key) const;
  std::optional<bool> getBool(std::string_view key) const;
  std::optional<float> getFloat(std::string_view key) const;
  std::optional<std::string> getString(std::string_view key) const;
  std::optional<Json> getObject(std::string_view key) const;

  int getIntOr(std::string_view key, int other) const {
    auto v = getInt(key);
    return v.has_value() ? *v : other;
  }

  bool getBoolOr(std::string_view key, bool other) const {
    auto v = getBool(key);
    return v.value_or(other);
  }

  float getFloatOr(std::string_view key, float other) const {
    auto v = getFloat(key);
    return v.value_or(other);
  }

  std::string getStringOr(std::string_view key, const std::string &other) const {
    auto v = getString(key);
    return v.value_or(other);
  }

  Json getObjectOr(std::string_view key, const Json &other) const {
    auto v = getObject(key);
    return v.value_or(other);
  }

  std::optional<std::vector<int>> getIntArray(std::string_view key) const;
  std::optional<std::vector<bool>> getBoolArray(std::string_view key) const;
  std::optional<std::vector<float>> getFloatArray(std::string_view key) const;
  std::optional<std::vector<std::string>> getStringArray(std::string_view key) const;
  std::optional<std::vector<Json>> getObjectArray(std::string_view key) const;

  void set(std::string_view key, int value);
  void set(std::string_view key, bool value);
  void set(std::string_view key, float value);
  void set(std::string_view key, std::string_view value);
  void set(std::string_view key, const Json &value);

  void setArray(std::string_view key, const std::vector<int> &value);
  void setArray(std::string_view key, const std::vector<bool> &value);
  void setArray(std::string_view key, const std::vector<float> &value);
  void setArray(std::string_view key, const std::vector<std::string> &value);
  void setArray(std::string_view key, const std::vector<Json> &value);

  std::string serialize() const;
  std::string serialize(std::string_view key) const;
};
