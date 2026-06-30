#pragma once
#include <algorithm>
#include <type_traits>
#include <vector>

namespace BlizzardDatabaseLib {
namespace Extension {

class Vector {
public:
  template <typename T>
  bool static IndexOf(std::vector<T> vector, T search, unsigned int &index) {
    auto interator = std::find(vector.begin(), vector.end(), search);

    if (interator != vector.cend()) {
      index = std::distance(vector.begin(), interator);
      return true;
    } else {
      return false;
    }
  }
};
} // namespace Extension
} // namespace BlizzardDatabaseLib
