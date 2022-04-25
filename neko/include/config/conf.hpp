#pragma once
#include "config/file.hpp"
#include "options/section.hpp"

namespace neko::config
{
  class cfg final
  {
  public:
    using file_type  = cfg_file;
    using file_name  = file_type::name_type;
    using line_type  = file_type::line_type;
    using name_type  = section::name_type;

  public:
    CLASS_SPECIALS_NODEFAULT_NOCOPY(cfg);

    explicit cfg(file_name fname) noexcept;

    explicit operator bool() const noexcept;

    const section* operator->() const noexcept;

    const section& operator*() const noexcept;

  private:
    void read() noexcept;
    bool parse() noexcept;

  private:
    file_type m_file;
    section   m_root;
  };
}