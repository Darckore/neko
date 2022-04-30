//
// Config parser
//

#pragma once
#include "config/parser/lex.hpp"
#include "config/options/section.hpp"

namespace neko::config
{
  //
  // Parser for configuration files
  //
  class parser final
  {
  public:
    //
    // The root section is always anonymous
    // This is a placeholder name assigned to it
    // No other section is the file can have this name, since technically,
    // it is grammatically invalid
    //
    static constexpr auto root_name = "~"sv;

  public:
    using value_type  = section;
    using option_type = value_type::value_type;
    using value_opt   = std::optional<value_type>;
    using size_type   = lex::size_type;
    using token_type  = lex::token;

  public:
    CLASS_SPECIALS_NONE(parser);

    //
    // Constructs a parser from a configuration file
    //
    explicit parser(cfg_file& file) noexcept;

    //
    // Checks whether the root section exists
    // This indicates parsing success
    //
    explicit operator bool() const noexcept;

  public:
    //
    // Passes ownership to the parsing result
    //
    value_type get() noexcept;

  private:
    //
    // Body: Section|Option
    // Returns the last rejected token type
    //
    token_type body() noexcept;

    //
    // Value: intNum|floatNum|boolTrue|boolFalse
    // OptionBody: Value , ... , Value
    // Returns the last rejected token type
    //
    token_type body(option_type& opt) noexcept;

    //
    // SectionBody: { Body }
    //
    bool section_body(value_type& root) noexcept;

    //
    // Section: .identifier SectionBody
    //
    bool section(token_type token) noexcept;

    //
    // OptionValues: { OptionBody }
    //
    bool option_values(option_type& opt) noexcept;

    //
    // Option: identifier OptionValues
    //
    bool option(token_type token) noexcept;
    
    //
    // Dispatches the current token type
    //
    bool parse(token_type token) noexcept;

    //
    // Performs parsing from the start of the file
    //
    bool parse() noexcept;

  private:
    //
    // Lexer used to obtain tokens
    //
    lex m_lexer;

    //
    // Parsing result
    // An empty optional if parsing failed
    //
    value_opt m_res;

    //
    // The current section considered 'root'
    // This is the section whose body is being parsed at the moment
    //
    value_type* m_root{};
  };
}