#pragma once

namespace neko::config
{
  namespace detail
  {
    template <typename T>
    concept val_type =
      !std::is_same_v<T, char>  &&
       (std::is_arithmetic_v<T> ||
        std::is_same_v<T, std::string_view>);
  }

  class value final
  {
  public:
    using bool_val   = bool;
    using int_val    = std::int64_t;
    using float_val  = float;
    using str_val    = std::string_view;
    using value_type = std::variant<
                                     bool_val,
                                     int_val,
                                     float_val,
                                     str_val
                                   >;

    template <typename T>
    static constexpr auto type_ok = std::is_same_v<T, bool_val> ||
                                    std::is_same_v<T, int_val> ||
                                    std::is_same_v<T, float_val> ||
                                    std::is_same_v<T, str_val>;

  public:
    CLASS_SPECIALS_NODEFAULT(value);

    constexpr value(detail::val_type auto v) noexcept :
      m_val{ v }
    { }

    value& operator=(detail::val_type auto v) noexcept
    {
      m_val = v;
    }

    template <detail::val_type T> requires (type_ok<T>)
    bool is() const noexcept
    {
      return std::holds_alternative<T>(m_val);
    }

    template <detail::val_type T> requires (type_ok<T>)
    const auto& get() const noexcept
    {
      return std::get<T>(m_val);
    }
    template <detail::val_type T> requires (type_ok<T>)
    auto& get() noexcept
    {
      return utils::mutate(std::as_const(*this).get<T>());
    }

    template <detail::val_type T> requires (type_ok<T>)
    auto try_get() const noexcept
    {
      return std::get_if<T>(&m_val);
    }
    template <detail::val_type T> requires (type_ok<T>)
    auto try_get() noexcept
    {
      return utils::mutate(std::as_const(*this).try_get<T>());
    }

  private:
    value_type m_val;
  };
}