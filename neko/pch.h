#pragma once

// std headers we'll most likely be using
#include <type_traits>
#include <concepts>
#include <utility>
#include <source_location>

#include <iostream>
#include <fstream>

#include <string>
#include <string_view>

#include <array>
#include <vector>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>

#include <optional>
#include <variant>

#include <filesystem>
#include <memory>
#include <new>

#include <format>

// just because it is often needed everywhere
using namespace std::literals;
namespace fsys = std::filesystem;

#include "utils/utils.hpp"

namespace neko
{
  using time_type  = float;
  using coord_type = float;

  using vec2 = utils::vector<coord_type, 2>;
  using vec3 = utils::vector<coord_type, 3>;
  using mat2 = utils::matrix<coord_type, 2, 2>;
  using mat3 = utils::matrix<coord_type, 3, 3>;

  using clock_type = utils::clock<time_type>;

  template <typename T>
  using pointer = std::unique_ptr<T>;
}