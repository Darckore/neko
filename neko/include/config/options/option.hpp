//
// Config option
//

#pragma once
#include "config/options/value.hpp"

namespace neko::config
{
  class section;

  //
  // Config option
  // Can contain one or more values (see value.hpp)
  //
  class option final
  {
  public:
    using value_type = value;
    using name_type  = std::string_view;
    using val_store  = std::vector<value_type>;
    using size_type  = val_store::size_type;

    //
    // Helper type defining a tuple of matching types
    //
    template <typename ...T>
      requires (utils::eq_all(true, value_type::type_ok<T>...))
    using tuple_type = std::tuple<T...>;

    //
    // Helper type for an optional value
    //
    template <typename T>
    using opt_type = std::optional<T>;

  public:
    CLASS_SPECIALS_NODEFAULT_NOCOPY(option);

    //
    // Constructs a named option belonging to the given section
    //
    option(name_type name, section& parent) noexcept;

    //
    // Returns an iterator to the values
    //
    auto begin() const noexcept
    {
      return m_values.begin();
    }

    //
    // Returns an iterator past the last value
    //
    auto end() const noexcept
    {
      return m_values.end();
    }

    //
    // Non-const version of begin
    //
    auto begin() noexcept
    {
      return m_values.begin();
    }

    //
    // Non-const version of end
    //
    auto end() noexcept
    {
      return m_values.end();
    }
    
    //
    // Returns the value at index
    // No checks performed
    //
    auto& operator[](size_type index) const noexcept
    {
      return m_values[index];
    }

    //
    // Non-const version of []
    //
    auto& operator[](size_type index) noexcept
    {
      return utils::mutate(std::as_const(*this).operator[](index));
    }

  public:
    //
    // Adds a value to the collection
    //
    void add_value(detail::val_type auto v)
    {
      m_values.emplace_back(v);
    }

    //
    // Returns the option's name
    //
    name_type name() const noexcept;

    //
    // Returns the section this option belongs to
    //
    const section& parent() const noexcept;

    //
    // Non-const version of parent
    //
    section& parent() noexcept;

    //
    // Returns the number of values associated with this option
    //
    size_type size() const noexcept;

  private:
    //
    // Helper which checks an sets the Ith element of the tuple
    // this option is being converted to
    //
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

    //
    // Implementation of convertions to tuples
    //
    template <typename Tuple, typename T, T... Seq>
    auto to_impl(std::integer_sequence<T, Seq...>) const noexcept
    {
      using res_type = opt_type<Tuple>;
      Tuple res{};

      const bool ok = (set_val<Seq>(res) && ...);
      return ok ? res : res_type{};
    }

  public:
    //
    // Tries to convert this option to the specified type
    // This type needs to be a structure whose fields have types and order
    // corresponding to those of this option's values
    // The struct must support tuple_size and tuple_element
    // Returns an empty optional is the conversion is impossible
    //
    template <typename T> requires
      requires {{ std::tuple_size_v<T> };}
    auto to() const noexcept -> opt_type<T>
    {
      constexpr auto tuple_size = std::tuple_size_v<T>;
      if (size() != tuple_size)
        return {};

      return to_impl<T>(std::make_index_sequence<tuple_size>{});
    }

    //
    // Tries to convert this option to a tuple of the specified types
    // Returns an empty optional if value types don't match those requested
    //
    template <typename ...T> requires (value_type::type_ok<T> && ...)
    auto to() const noexcept
    {
      return to<tuple_type<T...>>();
    }

  private:
    //
    // Owning (parent) config section
    //
    section*  m_parent{};

    //
    // The name of this option
    //
    name_type m_name;

    //
    // Value collection
    //
    val_store m_values;
  };


  //
  // A helper class an option can be converted to
  // Basically, this is just a wrapper over an std::tuple
  //
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


  //
  // Wrapper which acts as a base for classes
  // an option can be converted to
  // 
  // Let's say, we have an option like this: op{ 1, 1.0, true }
  // The corresponding tuple the option can be converted to is:
  // std::tuple<int64, float, bool>
  // 
  // Now, if we want to make a structure we can use rather than dealing with
  // awkward variant interface, we can simply:
  // 
  // struct myopt : opt_wrapper<int64, float, bool> { int64& i(){ return get<0>(); }... };
  // auto r = option.to<myopt>()
  // 
  // See TEST(conf, t_opt_custom)
  //
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
    const auto& get() const noexcept
    {
      NEK_ASSERT(*this);
      return m_value->template get<I>();
    }
    template <size_type I> requires (I < tuple_type::size)
    auto& get() noexcept
    {
      return utils::mutate(std::as_const(*this).get<I>());
    }

  private:
    value_type m_value;
  };


  //
  // An option whose only value is of the specified type
  // 
  // See TEST(conf, t_single)
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
      return base_type::template get<0>();
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