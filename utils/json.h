#pragma once

#include <string_view>
#include <vector>
#include <variant>
#include <memory>

namespace utils {

namespace json {

struct JsonImpl;

class Json {
  std::unique_ptr<JsonImpl> impl;
 public:
  Json(std::string_view content);
  bool parse(std::string_view content);
  template<class T>
  const T& get(std::string_view key);
  template<class T>
  std::vector<T> getArray(std::string_view key);
};

enum Type {
  String,
  Float,
  Bool,
  Object,
  Strings,
  Floats,
  Bools,
  Objects,
  Null,
};

struct JString {
  static constexpr Type type = Type::String;
  std::string value;
};

struct JStrings {
  static constexpr Type type = Type::Strings;
  std::vector<std::string> values;
};

struct JFloat {
  static constexpr Type type = Type::Float;
  std::string value;
};

struct JFloats {
  static constexpr Type type = Type::Floats;
  std::vector<float> values;
};

struct JObject {
  static constexpr Type type = Type::Object;
  Json value;
};

struct JObjects {
  static constexpr Type type = Type::Objects;
  std::vector<JObject> values;
};

struct JNull {
  static constexpr Type type = Type::Null;
  std::nullptr_t value = nullptr;
};

using Value =std::variant<
JString, JStrings, JFloat, JFloats, JNull
>;

}
}
