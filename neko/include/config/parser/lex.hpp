//
// Config lexer
//

#pragma once
#include "config/file.hpp"

namespace neko::config
{
  //
  // Lexer for the configuration parser
  //
  class lex final
  {
  public:
    using tok_value = cfg_file::line_type;
    using size_type = tok_value::size_type;
    using char_type = tok_value::value_type;
    using file_iter = cfg_file::iterator;

    //
    // Token produced by the lexer
    //
    struct token
    {
      //
      // Types of accepted tokens
      //
      enum class token_id : std::uint8_t
      {
        unknown,     // Empty or invalid token
        section,     // Section token: .identifier
        identifier,  // Any identifier. Starts with [_ letter], contains [_ letters digits]
        intNum,      // An integer. Starts with [+ - digit], contains digits
        floatNum,    // A float. Starts with [+ - digit], contains digits and exactly one .
        boolTrue,    // A bool true value, case-sensitive
        boolFalse,   // A bool false value, case-sensitive
        str,         // A string. Any sequence of characters enclosed in single quotes (')

        curlyOpen,  // An opening curly brace ({)
        curlyClose, // A closing curly brace (})
        comma       // A comma (,)
      };

      //
      // Checks whether the token is valid
      //
      constexpr explicit operator bool() const noexcept
      {
        return id != unknown;
      }

      //
      // Checks whether the token is of the specified type
      //
      constexpr auto is(token_id i) const noexcept
      {
        return id == i;
      }

      //
      // Performs token postprocessing and returns its value
      //
      tok_value postproc_val() const noexcept
      {
        return lex::unwrap(*this);
      }

      //
      // The part of the underlying buffer corresponding to this token
      //
      tok_value value;

      //
      // This token's id
      //
      token_id id{ unknown };

      using enum token_id;
    };

  public:
    //
    // Postprocesses the token value
    // - for token::section, removes the leading .
    // - for token::str, removes the leading and trailing '
    //
    static tok_value unwrap(token tok) noexcept;

  public:
    CLASS_SPECIALS_NONE(lex);

    //
    // Constructs a lexer from a configuration file
    //
    explicit lex(cfg_file& file) noexcept;

    //
    // Checks whether there's input to process (the file is not at its end)
    //
    explicit operator bool() const noexcept;

  public:
    //
    // Gets next token
    //
    token next() noexcept;

    //
    // Rewinds to the end of the file
    //
    void discard() noexcept;

  private:
    using tok_id = token::token_id;

    //
    // Checks if the upcoming token is an identifier
    //
    bool identifier() noexcept;
    
    //
    // Checks if the upcoming token is a numeric value
    // Returns the value type (intNum/floatNum)
    // or unknown if it is not a number
    //
    tok_id number() noexcept;

    //
    // Checks if the upcoming token is a boolean value
    // Returns the value type (boolTrue/boolFalse)
    // or unknown if it is not a bool
    //
    tok_id bool_val(tok_value v) noexcept;

    //
    // Checks if the upcoming token is a string value
    //
    bool string_val() noexcept;

    //
    // Same as operator bool
    //
    bool good() const noexcept;

    //
    // Peeks the next non-blank character from the file
    //
    void fpeek() noexcept;

    //
    // Consumes the characters from the file and returns the token
    // corresponding to them
    // The parameter specifies the type to assign to this token
    //
    token consume(token::token_id id) noexcept;

  private:
    //
    // The file being processed
    //
    cfg_file& m_file;

    //
    // An iterator to the beginning of the current token
    //
    file_iter m_from;

    //
    // An iterator past the end of the current token
    //
    file_iter m_to;
  };
}