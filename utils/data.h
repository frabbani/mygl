#pragma once

#include <cstdio>
#include <cstdint>

#include <string>
#include <vector>
#include <optional>

namespace utils {

template<typename T>
struct Stream {
  int pos = 0;
  int size = 0;
  T *data = nullptr;
  Stream(T *data_, uint32_t size_)
      :
      pos(0),
      size(size_),
      data(data_) {
  }
};

struct CharStream : public Stream<const char> {
  std::vector<std::pair<int, int> > lines;

  CharStream(const char *data_, uint32_t size_)
      :
      Stream(data_, size_) {
    int lastPos = 0;
    while (pos < size) {
      char c = data[pos];
      if (c == '\n') {
        lines.push_back( { lastPos, pos });
        lastPos = pos + 1;
      }
      pos++;
    }
    if (pos == size && pos > lastPos) {
      lines.push_back( { lastPos, pos - 1 });
    }
  }

  uint32_t lineCount() {
    return lines.size();
  }

  std::string getLine(uint32_t no) {
    if (no >= lines.size())
      return "";
    auto [a, b] = lines[no];
    return std::string(&data[a], b - a + 1);
  }

};

}
