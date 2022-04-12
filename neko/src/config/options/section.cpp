#include "config/options/section.hpp"

namespace neko::config
{
  // Special members
  section::section(name_type name) noexcept :
    section{ name, nullptr }
  { }

  section::section(name_type name, section* parent) noexcept :
    m_parent{ parent },
    m_name{ name }
  { }

  // Public members

  namespace detail
  {
    template <typename T>
    struct getter
    {
      using value_type = T;
      using name_type  = section::name_type;
      using cont_type = std::unordered_map<name_type, value_type>;

      getter(const cont_type& cont, name_type name) noexcept
      {
        if (auto it = cont.find(name); it != cont.end())
        {
          value = &it->second;
        }
      }

      const value_type* value{};
    };
  }

  section& section::add_section(name_type name)
  {
    auto item = m_subsections.emplace(name, section{ name, this });
    return item.first->second;
  }
  bool section::has_section(name_type name) const noexcept
  {
    return get_section(name);
  }
  const section* section::get_section(name_type name) const noexcept
  {
    return detail::getter{ m_subsections, name }.value;
  }
  section* section::get_section(name_type name) noexcept
  {
    return utils::mutate(std::as_const(*this).get_section(name));
  }
  
  option& section::add_option(name_type name)
  {
    auto item = m_options.emplace(name, option{ name, *this });
    return item.first->second;
  }
  bool section::has_option(name_type name) const noexcept
  {
    return get_option(name);
  }
  const option* section::get_option(name_type name) const noexcept
  {
    return detail::getter{ m_options, name }.value;
  }
  option* section::get_option(name_type name) noexcept
  {
    return utils::mutate(std::as_const(*this).get_option(name));
  }

  section* section::parent() noexcept
  {
    return utils::mutate(std::as_const(*this).parent());
  }
  const section* section::parent() const noexcept
  {
    return m_parent;
  }

  bool section::is_root() const noexcept
  {
    return !parent();
  }
}