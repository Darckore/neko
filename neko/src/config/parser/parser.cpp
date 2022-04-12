#include "config/parser/parser.hpp"
#include "config/options/option.hpp"

namespace neko::config
{
  namespace detail
  {
    using tok_type   = parser::token_type;
    using parser_val = parser::option_type::value_type;
    using int_type   = parser_val::int_val;
    using flt_type   = parser_val::float_val;
    
    template <typename T>
    using opt = std::optional<T>;

    template <typename T>
    inline constexpr auto is_int = std::is_same_v<T, int_type>;

    template <typename T>
    inline constexpr auto is_float = std::is_same_v<T, flt_type>;

    using str_type = lex::tok_value;

    inline auto is_open_brace(lex& l) noexcept
    {
      auto token = l.next();
      return token.is(tok_type::curlyOpen);
    }
    inline constexpr auto is_close_brace(tok_type tok) noexcept
    {
      return tok.is(tok_type::curlyClose);
    }

    template <typename T> requires (is_int<T> || is_float<T>)
    opt<T> to(str_type str) noexcept
    {
      auto begin = str.data();
      auto end = begin + str.length();

      T result{};
      auto convRes = std::from_chars(begin, end, result);
      if (convRes.ec != std::errc{ 0 })
        return {};

      return result;
    }

    auto to_int(str_type str) noexcept
    {
      return to<int_type>(str);
    }
    auto to_float(str_type str) noexcept
    {
      return to<flt_type>(str);
    }
  }

  // Special members

  parser::parser(cfg_file& file) noexcept :
    m_lexer{ file },
    m_res{ root_name },
    m_root{ &*m_res }
  {
    if (!parse())
      m_res.reset();
  }

  parser::operator bool() const noexcept
  {
    return m_res.has_value();
  }

  // Public members

  parser::value_type parser::get() noexcept
  {
    return std::move(*m_res);
  }

  // Private members

  parser::token_type parser::body() noexcept
  {
    token_type token;
    while (m_lexer)
    {
      token = m_lexer.next();
      if (!parse(token))
        break;
    }
    return token;
  }
  parser::token_type parser::body(option_type& opt) noexcept
  {
    token_type token;
    bool commaExpected = false;
    while (m_lexer)
    {
      token = m_lexer.next();
      if (commaExpected && !token.is(token_type::comma))
        break;

      switch (token.id)
      {
      case token_type::boolTrue:
        opt.add_value(true);
        commaExpected = true;
        continue;

      case token_type::boolFalse:
        opt.add_value(false);
        commaExpected = true;
        continue;

      case token_type::intNum:
        if (auto iv = detail::to_int(token.value))
        {
          opt.add_value(*iv);
          commaExpected = true;
          continue;
        }
        break;
      
      case token_type::floatNum:
        if (auto fv = detail::to_float(token.value))
        {
          opt.add_value(*fv);
          commaExpected = true;
          continue;
        }
        break;

      case token_type::str:
        opt.add_value(token.postproc_val());
        commaExpected = true;
        continue;
      
      case token_type::comma:
        if (commaExpected)
        {
          commaExpected = false;
          continue;
        }
        break;
      
      default:
        break;
      }

      break;
    }
    return token;
  }

  bool parser::section_body(value_type& root) noexcept
  {
    m_root = &root;
    if (!detail::is_open_brace(m_lexer))
      return false;

    auto token = body();
    m_root = m_root->parent();
    return detail::is_close_brace(token);
  }
  bool parser::section(token_type token) noexcept
  {
    if (!token.is(token_type::section))
      return false;

    auto name = token.postproc_val();
    auto&& sec = m_root->add_section(name);
    if (!section_body(sec))
    {
      m_lexer.discard();
      return false;
    }

    return true;
  }

  bool parser::option_values(option_type& opt) noexcept
  {
    if (!detail::is_open_brace(m_lexer))
      return false;

    return detail::is_close_brace(body(opt));
  }
  bool parser::option(token_type token) noexcept
  {
    if (!token.is(token_type::identifier))
      return false;

    auto name = token.postproc_val();
    auto&& opt = m_root->add_option(name);
    if (!option_values(opt))
    {
      m_lexer.discard();
      return false;
    }

    return true;
  }

  bool parser::parse(token_type token) noexcept
  {
    if (section(token))
    {
      return true;
    }

    if (m_lexer)
    {
      return option(token);
    }

    return false;
  }

  bool parser::parse() noexcept
  {
    while (m_lexer)
    {
      if (!parse(m_lexer.next()))
      {
        return false;
      }
    }

    return true;
  }
}