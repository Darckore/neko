//
// Config section
//

#pragma once
#include "config/options/option.hpp"

namespace neko::config
{
  //
  // Config section
  // Can contain an arbitrary number of options and subsections
  //
  class section final
  {
  public:
    using name_type  = option::name_type;
    using value_type = option;
    using opt_store  = std::unordered_map<name_type, value_type>;
    using sec_store  = std::unordered_map<name_type, section>;

  public:
    CLASS_SPECIALS_NODEFAULT_NOCOPY(section);

    //
    // Constructs a named section
    //
    explicit section(name_type name) noexcept;

    //
    // Constructs a named section, which is potentially a
    // subsection of another section
    //
    section(name_type name, section* parent) noexcept;

  public:
    //
    // Adds a subsection and returns a reference to it
    // If a subsection with the specified name exists, returns the
    // existing one
    //
    section& add_section(name_type name);

    //
    // Checks whether a subsection with the given name exists
    //
    bool has_section(name_type name) const noexcept;

    //
    // Returns a pointer to a subsection with the specified name
    // nullptr if one doesn't exist
    //
    const section* get_section(name_type name) const noexcept;

    //
    // Non-const version of get_section
    //
    section* get_section(name_type name) noexcept;

    //
    // Adds an option and returns a reference to it
    // If an option with the specified name exists, returns the
    // existing one
    //
    option& add_option(name_type name);

    //
    // Checks whether an option with the given name exists
    //
    bool has_option(name_type name) const noexcept;

    //
    // Returns a pointer to an option with the specified name
    // nullptr if one doesn't exist
    //
    const option* get_option(name_type name) const noexcept;

    //
    // Non-const version of get_option
    //
    option* get_option(name_type name) noexcept;

    //
    // Returns a pointer to the parent section
    // nullptr if this section is the root one
    //
    const section* parent() const noexcept;

    //
    // Non-const version of parent
    //
    section* parent() noexcept;

    //
    // Checks whether this section is the root (parent is null)
    //
    bool is_root() const noexcept;

  private:
    //
    // Parent section
    // nullptr is a valid value
    //
    section*  m_parent{};

    //
    // This section's name
    //
    name_type m_name;

    //
    // Subsections of this section
    //
    sec_store m_subsections;

    //
    // Options associated with this section
    //
    opt_store m_options;
  };
}