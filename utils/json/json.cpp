#include <algorithm>
#include <type_traits>

#include "json.hpp"
#include "../json.h"

using json = nlohmann::json;

template<class T> static constexpr bool matches(const json &j) {
  if (j.is_null())
    return false;
  if (std::is_same<T, int>::value)
    return j.is_number_integer();
  if (std::is_same<T, bool>::value)
    return j.is_boolean();
  if (std::is_same<T, float>::value)
    return j.is_number_float();
  if (std::is_same<T, std::string>::value)
    return j.is_string();
  if (std::is_same<T, Json>::value)
    return j.is_object();

  if (j.is_array() && j.size()) {
    const auto &v = j[0];
    if (std::is_same<T, std::vector<int>>::value)
      return v.is_number_integer();
    if (std::is_same<T, std::vector<bool>>::value)
      return v.is_boolean();
    if (std::is_same<T, std::vector<float>>::value)
      return v.is_number_float();
    if (std::is_same<T, std::vector<std::string>>::value)
      return v.is_string();
    if (std::is_same<T, std::vector<Json>>::value)
      return v.is_object();
  }

  return false;
}

template<class T> static constexpr bool isValue() {
  return std::is_same<T, int>::value || std::is_same<T, bool>::value || std::is_same<T, float>::value || std::is_same<T, std::string>::value;
}

struct Json;

struct JsonImpl {
  struct Reader {
    const json &j;
    Reader(const json &j_)
        :
        j(j_) {
    }
    void operator()(int &value) {
      value = j;
    }
    void operator()(float &value) {
      value = j;
    }
    void operator()(bool &value) {
      value = j;
    }
    void operator()(std::string &value) {
      value = j;
    }
    void operator()(Json &value) {
      value.impl = std::make_unique<JsonImpl>(j);
    }
  };
  json j;

  ~JsonImpl() = default;
  explicit JsonImpl(json j_)
      :
      j(std::move(j_)) {
  }
  explicit JsonImpl(std::string_view content)
      :
      j(json::parse(content)) {
  }

  std::optional<json> find(std::string_view key) const {
    if( !key.size())
      return j;
    auto it = j.find( key );
    if( it == j.end() )
      return std::nullopt;
    return j[key];
  }

  template<class T> bool get(T &value, std::string_view key) const {
    auto v = find(key);
    if (v.has_value() && matches<T>(v.value())) {
      JsonImpl::Reader(v.value())(value);
      return true;
    }
    return false;
  }

  template<class T> bool getArray(std::vector<T> &values, std::string_view key) const {
    auto v = find(key);
    if (v.has_value() && matches<std::vector<T>>(*v)) {
      T t;
      values.reserve(v->size());
      values.clear();
      for (const auto &j : *v) {
        JsonImpl::Reader r(j);
        r(t);
        values.push_back(t);
      }
      return true;
    }
    return false;
  }

};

Json::Json()
    :
    impl(std::make_unique<JsonImpl>(json::object())) {
}

Json::Json(std::string_view content)
    :
    impl(std::make_unique<JsonImpl>(content)) {
}

Json::~Json() {
  impl = nullptr;
}

Json::Json(const Json &rhs)
    :
    impl(std::make_unique<JsonImpl>(rhs.impl->j)) {
}

void Json::operator =(const Json &rhs){
  impl = std::make_unique<JsonImpl>(rhs.impl->j);
}

std::string Json::serialize() const {
  return impl->j.dump();
}

std::string Json::serialize(std::string_view key) const {
  auto j = impl->j[key];
  if (j.is_null())
    return "";
  return j.dump();
}

std::optional<int> Json::getInt(std::string_view key) const {
  int val;
  if (!impl->get<int>(val, key))
    return std::nullopt;
  return val;
}

std::optional<bool> Json::getBool(std::string_view key) const {
  bool val;
  if (!impl->get<bool>(val, key))
    return std::nullopt;
  return val;
}

std::optional<float> Json::getFloat(std::string_view key) const {
  float val;
  if (!impl->get<float>(val, key))
    return std::nullopt;
  return val;
}

std::optional<std::string> Json::getString(std::string_view key) const {
  std::string val;
  if (!impl->get<std::string>(val, key))
    return std::nullopt;
  return val;
}

std::optional<Json> Json::getObject(std::string_view key) const {
  Json val;
  if (!impl->get<Json>(val, key))
    return std::nullopt;
  return val;
}

std::optional<std::vector<int>> Json::getIntArray(std::string_view key) const {
  std::vector<int> values;
  if (impl->getArray<int>(values, key))
    return values;
  return std::nullopt;
}

std::optional<std::vector<bool>> Json::getBoolArray(std::string_view key) const {
  std::vector<bool> values;
  if (impl->getArray<bool>(values, key))
    return values;
  return std::nullopt;
}

std::optional<std::vector<float>> Json::getFloatArray(std::string_view key) const {
  std::vector<float> values;
  if (impl->getArray<float>(values, key))
    return values;
  return std::nullopt;
}

std::optional<std::vector<std::string>> Json::getStringArray(std::string_view key) const {
  std::vector<std::string> values;
  if (impl->getArray<std::string>(values, key))
    return values;
  return std::nullopt;
}

std::optional<std::vector<Json>> Json::getObjectArray(std::string_view key) const {
  std::vector<Json> values;
  if (impl->getArray<Json>(values, key))
    return values;
  return std::nullopt;
}

void Json::set(std::string_view key, int value) {
  if (key.length())
    impl->j[key] = value;
  else
    impl->j = value;
}

void Json::set(std::string_view key, bool value) {
  if (key.length())
    impl->j[key] = value;
  else
    impl->j = value;
}

void Json::set(std::string_view key, float value) {
  if (key.length())
    impl->j[key] = value;
  else
    impl->j = value;
}

void Json::set(std::string_view key,std::string_view value) {
  if (key.length())
    impl->j[key] = value;
  else
    impl->j = value;
}

void Json::set(std::string_view key, const Json &value) {
  if (key.length())
    impl->j[key] = value.impl->j;
  else
    impl->j = value.impl->j;
}

void Json::setArray(std::string_view key, const std::vector<int> &values) {
  auto setarr = [&](json &j) {
    j = json::array();
    for (auto v : values)
      j.push_back(v);
  };
  setarr(key.length() ? impl->j[key] : impl->j);

}

void Json::setArray(std::string_view key, const std::vector<bool> &values) {
  auto setarr = [&](json &j) {
    j = json::array();
    for (auto v : values)
      j.push_back(v);
  };
  setarr(key.length() ? impl->j[key] : impl->j);
}

void Json::setArray(std::string_view key, const std::vector<float> &values) {
  auto setarr = [&](json &j) {
    j = json::array();
    for (auto v : values)
      j.push_back(v);
  };
  setarr(key.length() ? impl->j[key] : impl->j);
}

void Json::setArray(std::string_view key, const std::vector<std::string> &values) {
  auto setarr = [&](json &j) {
    j = json::array();
    for (auto v : values)
      j.push_back(v);
  };
  setarr(key.length() ? impl->j[key] : impl->j);
}

void Json::setArray(std::string_view key, const std::vector<Json> &values) {
  auto setarr = [&](json &j) {
    j = json::array();
    for (const auto &v : values)
      j.push_back(v.impl->j);
  };
  setarr(key.length() ? impl->j[key] : impl->j);
}

