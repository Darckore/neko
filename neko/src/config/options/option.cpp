#include "config/options/option.hpp"

namespace neko::config
{
  // Special members

  option::option(name_type name, section& parent) noexcept :
    m_parent{ &parent },
    m_name{ name }
  {
  }

  // Public members

  option::name_type option::name() const noexcept
  {
    return m_name;
  }

  section& option::parent() noexcept
  {
    return utils::mutate(std::as_const(*this).parent());
  }
  const section& option::parent() const noexcept
  {
    return *m_parent;
  }

  option::size_type option::size() const noexcept
  {
    return m_values.size();
  }
}