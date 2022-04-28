//
// Config container
//

#pragma once
#include "config/file.hpp"
#include "options/section.hpp"

namespace neko::config
{
  //
  // Config container
  // Maintains a parsed configuration file and provides
  // an interface to the root section
  //
  class cfg final
  {
  public:
    using file_type  = cfg_file;
    using file_name  = file_type::name_type;
    using line_type  = file_type::line_type;
    using name_type  = section::name_type;

  public:
    CLASS_SPECIALS_NODEFAULT_NOCOPY(cfg);

    //
    // Constructs configuration from a file
    //
    explicit cfg(file_name fname) noexcept;

    //
    // Checks whether the configuration is valid
    //
    explicit operator bool() const noexcept;

    //
    // Allows accessing members of the root section
    //
    const section* operator->() const noexcept;

    //
    // Returns a reference to the root section
    //
    const section& operator*() const noexcept;

  private:
    //
    // Tries to parse the file
    // If that fails, discards the file contents and
    // transfers to invalid state
    //
    void read() noexcept;

    //
    // Parses the file
    //
    bool parse() noexcept;

  private:
    //
    // The config file being parsed
    //
    file_type m_file;

    //
    // The root section
    //
    section   m_root;
  };
}