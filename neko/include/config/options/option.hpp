#pragma once
#include "config/options/value.hpp"

namespace neko::config
{
  class section;

  class option
  {
  public:
    using value_type = value;
    using name_type  = std::string_view;
    using val_store  = std::vector<value_type>;
    using size_type  = val_store::size_type;

    template <typename ...T>
      requires (utils::eq_all(true, value_type::type_ok<T>...))
    using tuple_type = std::tuple<T...>;

    template <typename T>
    using opt_type = std::optional<T>;

  public:
    CLASS_SPECIALS_NODEFAULT_NOCOPY(option);

    option(name_type name, section& parent) noexcept;

    auto begin() const noexcept
    {
      return m_values.begin();
    }
    auto end() const noexcept
    {
      return m_values.end();
    }
    auto begin() noexcept
    {
      return m_values.begin();
    }
    auto end() noexcept
    {
      return m_values.end();
    }
    
    auto& operator[](size_type index) const noexcept
    {
      return m_values[index];
    }
    auto& operator[](size_type index) noexcept
    {
      return utils::mutate(std::as_const(*this).operator[](index));
    }

  public:
    void add_value(detail::val_type auto v)
    {
      m_values.emplace_back(v);
    }

    name_type name() const noexcept;

    section& parent() noexcept;
    const section& parent() const noexcept;

    size_type size() const noexcept;

  private:
    template <size_type I, typename Tuple> requires
      requires {{ std::tuple_element<I, Tuple>{} };}
    bool set_val(Tuple& t) const noexcept
    {
      using std::get;
      using val_type = std::tuple_element_t<I, Tuple>;
      if (auto valPtr = (*this)[I].template try_get<val_type>())
      {
        get<I>(t) = *valPtr;
        return true;
      }
      return false;
    }

    template <typename Tuple, typename T, T... Seq>
    auto to_impl(std::integer_sequence<T, Seq...>) const noexcept
    {
      using res_type = opt_type<Tuple>;
      Tuple res{};

      const bool ok = (set_val<Seq>(res) && ...);
      return ok ? res : res_type{};
    }

  public:
    template <typename T> requires
      requires {{ std::tuple_size_v<T> };}
    auto to() const noexcept -> opt_type<T>
    {
      constexpr auto tuple_size = std::tuple_size_v<T>;
      if (size() != tuple_size)
        return {};

      return to_impl<T>(std::make_index_sequence<tuple_size>{});
    }

    template <typename ...T> requires (value_type::type_ok<T> && ...)
    auto to() const noexcept
    {
      return to<tuple_type<T...>>();
    }

  private:
    section*  m_parent{};
    name_type m_name;
    val_store m_values;
  };

  template <typename ...T>
    requires (utils::eq_all(true, value::type_ok<T>...))
  class opt_tuple
  {
  public:
    using value_type = std::tuple<T...>;
    using size_type  = std::size_t;
    static constexpr auto size = std::tuple_size_v<value_type>;

  public:
    CLASS_SPECIALS_ALL(opt_tuple);

    constexpr opt_tuple(T&& ...args) noexcept :
      m_val{ std::forward<T>(args)... }
    {}

    constexpr opt_tuple(value_type val) noexcept :
      m_val{ std::move(val) }
    {}

    template <size_type I> requires (I < size)
    constexpr auto& get() const noexcept
    {
      return std::get<I>(m_val);
    }
    template <size_type I> requires (I < size)
    constexpr auto& get() noexcept
    {
      return utils::mutate(std::as_const(*this).get<I>());
    }

  private:
    value_type m_val;
  };

  template <typename ...T>
  struct std::tuple_size<opt_tuple<T...>>
  {
    using tuple_type = neko::config::opt_tuple<T...>;
    static constexpr auto value = tuple_type::size;
  };
  template <std::size_t I, typename ...T>
  struct std::tuple_element<I, opt_tuple<T...>>
  {
    using tuple_type = neko::config::opt_tuple<T...>;
    using type = tuple_element_t<I, typename tuple_type::value_type>;
  };
  template <std::size_t I, typename ...T>
  constexpr decltype(auto) get(opt_tuple<T...>& t) noexcept
  {
    using tuple_type = neko::config::opt_tuple<T...>;
    return t.template get<I>();
  }
  template <std::size_t I, typename ...T>
  constexpr decltype(auto) get(const opt_tuple<T...>& t) noexcept
  {
    using tuple_type = neko::config::opt_tuple<T...>;
    return t.template get<I>();
  }

  template <typename ...T>
  class opt_wrapper
  {
  public:
    using tuple_type = opt_tuple<T...>;
    using value_type = option::opt_type<tuple_type>;
    using size_type  = typename tuple_type::size_type;

  public:
    CLASS_SPECIALS_NODEFAULT(opt_wrapper);

    opt_wrapper(const option& opt) noexcept :
      m_value { opt.template to<tuple_type>() }
    { }

    explicit operator bool() const noexcept
    {
      return static_cast<bool>(m_value);
    }

  public:
    constexpr auto size() const noexcept
    {
      return tuple_type::size;
    }

  protected:
    template <size_type I> requires (I < tuple_type::size)
    auto get() const noexcept
    {
      using res_type = std::add_pointer_t<std::tuple_element_t<I, tuple_type>>;
      return m_value ? &m_value->template get<I>() : res_type{};
    }
    template <size_type I> requires (I < tuple_type::size)
    auto get() noexcept
    {
      return utils::mutate(std::as_const(*this).get<I>());
    }

  private:
    value_type m_value;
  };

  //
  // An option whose only value is of the specified type
  //
  template <detail::val_type T>
  class single_opt : public opt_wrapper<T>
  {
  public:
    using base_type = opt_wrapper<T>;

  public:
    CLASS_SPECIALS_NODEFAULT(single_opt);

    single_opt(const option& opt) noexcept :
      base_type{ opt }
    { }

    const auto& value() const noexcept
    {
      NEK_ASSERT(*this);
      return *base_type::template get<0>();
    }
    auto& value() noexcept
    {
      return utils::mutate(std::as_const(*this).value());
    }
  };

  using int_opt    = single_opt<value::int_val>;
  using float_opt  = single_opt<value::float_val>;
  using bool_opt   = single_opt<value::bool_val>;
  using string_opt = single_opt<value::str_val>;
}