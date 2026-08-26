#pragma once

#include <initializer_list>
#include <string_view>

#include <nlohmann/json.hpp>

namespace json_utils
{
  using json = nlohmann::json;

  template <typename T>
  bool read(
      const json &data,
      T &destination,
      std::initializer_list<std::string_view> path)
  {
    const json *current = &data;

    for (const auto &key : path)
    {
      if (!current->is_object())
        return false;

      const auto it = current->find(key);

      if (it == current->end())
        return false;

      current = &*it;
    }

    try
    {
      destination = current->get<T>();
      return true;
    }
    catch (const json::exception &)
    {
      return false;
    }
  }
}