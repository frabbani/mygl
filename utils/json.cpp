#include "json.hpp"

#include "json.h"

using json = nlohmann::json;

#define ASSERT static_assert( std::is_same<std::string, T>::value || \
                              std::is_same<int, T>::value || \
                              std::is_same<uint32_t, T>::value || \
                              std::is_same<float, T>::value || \
                              std::is_same<bool, T>::value || \
                              std::is_same<json, T>::value);

template< class T>
struct SameAs( const json& obj ) {
  ASSERT
};

struct JsonImpl {
  json j;
  json v;
  JsonImpl(std::string_view s)
      :
      j(json::parse(s)) {
  }

  void find(std::string_view key) {
    v = j[key];
  }

  template<class T>
  bool get(T &value, std::function<bool()> check) {
    ASSERT
    if (check()) {
      scalar = v;
      return true;
    }
    return false;
  }

  template<class T>
  bool get(std::vector<T> &array, std::function<bool()> check) {
    ASSERT
    if (check()) {
      scalar = v;
      return true;
    }
    return false;
  }

  bool get(std::string &str) {
    if (v.is_string()) {
      str = v;
      return true;
    }
    return false;
  }
  bool get(std::vector<std::string> &strs) {
    if (v.is_array() && v.is_string()) {
      strs = v;
      return true;
    }
    return false;
  }
  bool get(std::string_view k, float &fl) {
    if (v.is_number_float()) {
      fl = v;
      return true;
    }
    return false;
  }

  bool get(std::string_view k, std::vector<float> &fls) {
    if (v.is_array()) {
      fls = std::vector<float>(v.size());
      for (auto f : v) {
        if (f.is_number_float())
          fls.push_back(f);
      }
      return true;
    }
  }

  void get(std::string_view k, int &jint) {
    auto v = j[k];
    if (v.is_number_integer()) {
      jint = v;
    }
  }

  void get(std::string_view k, std::vector<uint32> &juints) {
    auto v = j[k];
    printf(" v: %s\n", v.dump().c_str());
    printf(" * type '%s'\n", v.type_name());

    if (v.is_array()) {
      printf(" * size '%d'\n", (int) v.size());
      juints = std::vector<uint32>(v.size());
      for (int i = 0; i < int(v.size()); i++)
        juints[i] = v[i];
    }
  }
}
;

class Json {
  std::unique_ptr<JsonImpl> impl;
 public:
  Json(std::string_view content)
      :
      impl(std::make_unique<JsonImpl>(content)) {
  }
  template<class T>
  std::optional<T> get(std::string_view key) {
    if (impl->j[key].is_null())
      return std::nullopt;
    if (std::is_same<T, uint32>::value) {
      T v;
      impl->get(key, v);
      return v;
    }
    if (std::is_same<T, std::vector<uint32>>::value) {
      T v;
      impl->get(key, v);
      return v;
    }
    return std::nullopt;
  }
};
