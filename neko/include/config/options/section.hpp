#pragma once
#include "config/options/option.hpp"

namespace neko::config
{
  class section final
  {
  public:
    using name_type  = option::name_type;
    using value_type = option;
    using opt_store  = std::unordered_map<name_type, value_type>;
    using sec_store  = std::unordered_map<name_type, section>;

  public:
    CLASS_SPECIALS_NODEFAULT_NOCOPY(section);

    explicit section(name_type name) noexcept;

    section(name_type name, section* parent) noexcept;

  public:
    section& add_section(name_type name);
    bool has_section(name_type name) const noexcept;
    const section* get_section(name_type name) const noexcept;
    section* get_section(name_type name) noexcept;

    option& add_option(name_type name);
    bool has_option(name_type name) const noexcept;
    const option* get_option(name_type name) const noexcept;
    option* get_option(name_type name) noexcept;

    section* parent() noexcept;
    const section* parent() const noexcept;

    bool is_root() const noexcept;

  private:
    section*  m_parent{};
    name_type m_name;
    sec_store m_subsections;
    opt_store m_options;
  };
}