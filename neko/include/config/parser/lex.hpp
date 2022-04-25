#pragma once
#include "config/file.hpp"

namespace neko::config
{
  class lex final
  {
  public:
    using tok_value = cfg_file::line_type;
    using size_type = tok_value::size_type;
    using char_type = tok_value::value_type;
    using file_iter = cfg_file::iterator;

    struct token
    {
      enum class token_id : std::uint8_t
      {
        unknown,
        section,
        identifier,
        intNum,
        floatNum,
        boolTrue,
        boolFalse,
        str,

        curlyOpen,
        curlyClose,
        comma
      };

      constexpr explicit operator bool() const noexcept
      {
        return id != unknown;
      }

      constexpr auto is(token_id i) const noexcept
      {
        return id == i;
      }

      tok_value postproc_val() const noexcept
      {
        return lex::unwrap(*this);
      }

      tok_value value;
      token_id id{ unknown };

      using enum token_id;
    };

  public:
    static tok_value unwrap(token tok) noexcept;

  public:
    CLASS_SPECIALS_NONE(lex);

    explicit lex(cfg_file& file) noexcept;

    explicit operator bool() const noexcept;

  public:
    token next() noexcept;
    void discard() noexcept;

  private:
    using tok_id = token::token_id;

    bool identifier() noexcept;
    
    tok_id number() noexcept;
    tok_id bool_val(tok_value v) noexcept;

    bool string_val() noexcept;

    bool good() const noexcept;
    void fpeek() noexcept;
    token consume(token::token_id id) noexcept;

  private:
    cfg_file& m_file;
    file_iter m_from;
    file_iter m_to;
  };
}