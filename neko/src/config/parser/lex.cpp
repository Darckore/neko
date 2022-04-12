#include "config/parser/lex.hpp"

namespace neko::config
{
  namespace detail
  {
    constexpr auto dot    = '.';
    constexpr auto comma  = ',';
    constexpr auto cOpen  = '{';
    constexpr auto cClose = '}';
    constexpr auto quote  = '\'';
    constexpr auto plus   = '+';
    constexpr auto minus  = '-';
    constexpr auto uscore = '_';
    constexpr auto eol    = '\n';

    constexpr auto bTrue  = "true"sv;
    constexpr auto bFalse = "false"sv;

    using char_type = lex::char_type;

    constexpr auto is_id_char(char_type c) noexcept
    {
      return c == uscore || std::isalnum(c);
    }
    constexpr auto is_num_start(char_type c) noexcept
    {
      return utils::eq_any(c, plus, minus) || std::isdigit(c);
    }
    constexpr auto is_delimiter(char_type c) noexcept
    {
      return utils::eq_any(c, comma, cOpen, cClose) ||
             std::isspace(c);
    }
  }

  // Statics

  lex::tok_value lex::unwrap(token tok) noexcept
  {
    if (tok.is(token::section))
      return utils::ltrim(tok.value, detail::dot);

    if (tok.is(token::str))
      return utils::trim(tok.value, detail::quote);

    return tok.value;
  }

  // Special members

  lex::lex(cfg_file& file) noexcept :
    m_file{ file }
  {
    fpeek();
  }

  lex::operator bool() const noexcept
  {
    return good();
  }

  // Public members

  lex::token lex::next() noexcept
  {
    if (!m_file)
      return {};

    while (good())
    {
      const auto c = *m_to;

      if (c == detail::dot)
      {
        ++m_to;
        if (identifier())
        {
          return consume(token::section);
        }
        break;
      }
      if (identifier())
      {
        return consume(token::identifier);
      }
      if (const auto id = number(); id != token::unknown)
      {
        return consume(id);
      }
      if (string_val())
      {
        return consume(token::str);
      }

      if (c == detail::comma)
      {
        ++m_to;
        return consume(token::comma);
      }
      if (c == detail::cOpen)
      {
        ++m_to;
        return consume(token::curlyOpen);
      }
      if (c == detail::cClose)
      {
        ++m_to;
        return consume(token::curlyClose);
      }

      break;
    }

    discard();
    return {};
  }
  void lex::discard() noexcept
  {
    m_from = m_file.end();
    m_to   = m_file.end();
  }

  // Private members

  bool lex::identifier() noexcept
  {
    if (!good() || !std::isalpha(*m_to))
      return false;

    ++m_to;
    while (good())
    {
      const auto c = *m_to;

      if (detail::is_delimiter(c))
        break;

      if (!detail::is_id_char(c))
        return false;

      ++m_to;
    }

    return true;
  }

  lex::tok_id lex::number() noexcept
  {
    constexpr auto fail = token::unknown;
    if (!good() || !detail::is_num_start(*m_to))
      return fail;

    ++m_to;
    bool hasDot = false;
    auto ret = token::intNum;
    while (good())
    {
      const auto c = *m_to;

      if (detail::is_delimiter(c))
        break;

      if (c == detail::dot)
      {
        if (!hasDot)
        {
          hasDot = true;
          ret = token::floatNum;
          ++m_to;
          continue;
        }

        return fail;
      }

      if (!std::isdigit(c))
        return fail;

      ++m_to;
    }

    return ret;
  }
  lex::tok_id lex::bool_val(tok_value v) noexcept
  {
    if (v == detail::bTrue)
      return token::boolTrue;

    if (v == detail::bFalse)
      return token::boolFalse;

    return token::unknown;
  }

  bool lex::string_val() noexcept
  {
    if (!good() || *m_to != detail::quote)
      return false;

    ++m_to;
    while (good())
    {
      const auto c = *m_to;

      if (c == detail::eol)
        return false;

      if (c == detail::quote)
      {
        ++m_to;
        break;
      }

      ++m_to;
    }

    return true;
  }

  bool lex::good() const noexcept
  {
    return m_to != m_file.end();
  }
  void lex::fpeek() noexcept
  {
    if (m_file)
    {
      m_file.peek();
      m_from = m_file.begin();
      m_to = m_from;
    }
  }
  lex::token lex::consume(token::token_id id) noexcept
  {
    auto val = m_file.consume(m_to);
    if (id == token::identifier)
    {
      if (const auto boolId = bool_val(val);
                     boolId != token::unknown)
      {
        id = boolId;
      }
    }

    token ret{ val, id };
    fpeek();
    return ret;
  }
}