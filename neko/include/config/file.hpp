#pragma once

namespace neko::config
{
  class cfg_file final
  {
  public:
    using name_type   = fsys::path;
    using line_type   = std::string_view;
    using char_type   = line_type::value_type;
    using in_type     = std::ifstream;
    using out_type    = std::ofstream;
    using buf_type    = std::string;
    using iterator    = buf_type::const_iterator;

  public:
    CLASS_SPECIALS_NODEFAULT_NOCOPY(cfg_file);

    explicit cfg_file(name_type fname) noexcept;

    explicit operator bool() const noexcept;

  public:
    void rewind() noexcept;
    line_type consume(iterator upto) noexcept;

    void discard() noexcept;

    line_type line() noexcept;

    char_type peek() noexcept;

    auto begin() const noexcept
    {
      return m_cur;
    }
    auto end() const noexcept
    {
      return m_buf.end();
    }

  private:
    void read() noexcept;

  private:
    name_type m_name;
    buf_type  m_buf;
    iterator  m_cur;
  };
}