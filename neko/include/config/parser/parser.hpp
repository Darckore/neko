#pragma once
#include "config/parser/lex.hpp"
#include "config/options/section.hpp"

namespace neko::config
{
  class parser final
  {
  public:
    static constexpr auto root_name = "~"sv;

  public:
    using value_type  = section;
    using option_type = value_type::value_type;
    using value_opt   = std::optional<value_type>;
    using size_type   = lex::size_type;
    using token_type  = lex::token;

  public:
    CLASS_SPECIALS_NONE(parser);

    explicit parser(cfg_file& file) noexcept;

    explicit operator bool() const noexcept;

  public:
    value_type get() noexcept;

  private:
    token_type body() noexcept;
    token_type body(option_type& opt) noexcept;

    bool section_body(value_type& root) noexcept;
    bool section(token_type token) noexcept;

    bool option_values(option_type& opt) noexcept;
    bool option(token_type token) noexcept;
    
    bool parse(token_type token) noexcept;

    bool parse() noexcept;

  private:
    lex m_lexer;
    value_opt m_res;
    value_type* m_root{};
  };
}