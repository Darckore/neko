//
// Config file
//

#pragma once

namespace neko::config
{
  //
  // Configuration file
  // Handles file I/O and maintains an underlying string buffer
  //
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

    //
    // Constructs a file from the path
    //
    explicit cfg_file(name_type fname) noexcept;

    //
    // Checks if there's data to read
    //
    explicit operator bool() const noexcept;

  public:
    //
    // Rewinds to the beginning of the buffer
    //
    void rewind() noexcept;

    //
    // Rewinds the current buffer iterator to the one passed as the parameter
    // Returns the character sequence between
    // the original and the resulting positions
    //
    line_type consume(iterator upto) noexcept;

    //
    // Discards the buffer
    //
    void discard() noexcept;

    //
    // Reads a line and advances the buffer iterator
    //
    line_type line() noexcept;

    //
    // Returns the next non-blank character
    //
    char_type peek() noexcept;

    //
    // Returns the current buffer iterator
    //
    auto begin() const noexcept
    {
      return m_cur;
    }

    //
    // Returns an iterator to the end of the buffer
    //
    auto end() const noexcept
    {
      return m_buf.end();
    }

  private:
    //
    // Reads the entire file into the underlying buffer
    //
    void read() noexcept;

  private:
    //
    // Path to the file
    //
    name_type m_name;

    //
    // The underlying buffer
    //
    buf_type  m_buf;

    //
    // The current buffer read position
    //
    iterator  m_cur;
  };
}