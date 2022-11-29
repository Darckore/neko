//
// Config option value
//

#pragma once

namespace neko::config
{
  namespace detail
  {
    //
    // Defines types allowed for a config value
    //
    template <typename T>
    concept val_type =
      !std::is_same_v<T, char>  &&
       (std::is_arithmetic_v<T> ||
        std::is_same_v<T, std::string_view>);
  }

  //
  // Config option value. Can be any of these types:
  // - a 64-bit signed integer
  // - a 32-bit float
  // - a bool
  // - a string (std::string_view pointing into the file buffer)
  //
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

    //
    // Constructs the object from one of allowed types
    //
    constexpr value(detail::val_type auto v) noexcept :
      m_val{ v }
    { }

    //
    // Assigns a new value potentially changing the underlying type
    //
    value& operator=(detail::val_type auto v) noexcept
    {
      m_val = v;
      return *this;
    }

    //
    // Checks whether the value is of the specified type
    //
    template <detail::val_type T> requires (type_ok<T>)
    bool is() const noexcept
    {
      return std::holds_alternative<T>(m_val);
    }

    //
    // Gets the value as the specified type
    // No checks performed
    //
    template <detail::val_type T> requires (type_ok<T>)
    const auto& get() const noexcept
    {
      return std::get<T>(m_val);
    }

    //
    // Non-const version of get. Allows reassignment
    //
    template <detail::val_type T> requires (type_ok<T>)
    auto& get() noexcept
    {
      return utils::mutate(std::as_const(*this).get<T>());
    }

    //
    // Tries to get the value as the specified type
    // Returns a pointer is succeedes, nullptr otherwise
    //
    template <detail::val_type T> requires (type_ok<T>)
    auto try_get() const noexcept
    {
      return std::get_if<T>(&m_val);
    }

    //
    // Non-const version of try_get. Allows reassignment
    //
    template <detail::val_type T> requires (type_ok<T>)
    auto try_get() noexcept
    {
      return utils::mutate(std::as_const(*this).try_get<T>());
    }

  private:
    //
    // A variant holding one of the allowed types
    //
    value_type m_val;
  };
}