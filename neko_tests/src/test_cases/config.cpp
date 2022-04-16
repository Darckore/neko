#include "config/conf.hpp"
#include "config/parser/lex.hpp"

using namespace neko::config;
using token = lex::token;

namespace engine_tests
{
  namespace detail
  {
    void check_token(lex& l, std::string_view str, token::token_id id)
    {
      auto tok = l.next();
      ASSERT_TRUE(tok.is(id));
      ASSERT_EQ(tok.value, str);
    }

    auto get_option(const section& s, std::string_view name)
    {
      auto ret = s.get_option(name);
      auto check = [ret, name]()
      {
        ASSERT_TRUE(ret);
        ASSERT_EQ(name, ret->name());
      };

      check();
      return ret;
    }

    template <typename T>
    void check_opt_value(const option& opt, std::size_t size, std::size_t idx, T expected)
    {
      ASSERT_EQ(size, opt.size());
      ASSERT_LT(idx, size);

      auto v = opt[idx].try_get<T>();
      ASSERT_TRUE(v);

      if constexpr (std::is_floating_point_v<T>)
      {
        ASSERT_DOUBLE_EQ(*v, expected);
      }
      else
      {
        ASSERT_EQ(*v, expected);
      }
    }
  }

  TEST(conf, t_option)
  {
    section s{ "empty"sv };

    option opt{ "dummy"sv, s };
    EXPECT_EQ(opt.name(), "dummy"sv);
    EXPECT_EQ(&opt.parent(), &s);
    EXPECT_EQ(opt.size(), 0);

    opt.add_value(10);
    EXPECT_EQ(opt.size(), 1);

    auto val = opt[0];
    EXPECT_FALSE((val.try_get<value::bool_val>()));
    EXPECT_FALSE((val.try_get<value::float_val>()));

    ASSERT_TRUE((val.try_get<value::int_val>()));
    EXPECT_EQ(10, (val.get<value::int_val>()));
  }

  TEST(conf, t_section_add_get)
  {
    section s{ "glob"sv };

    constexpr auto name1 = "dummy"sv;
    auto&& dummy = s.add_section(name1);
    EXPECT_TRUE(s.has_section(name1));

    auto&& anotherDummy = s.add_section(name1);
    EXPECT_TRUE(&dummy == &anotherDummy);

    auto dummyLookup = s.get_section(name1);
    EXPECT_TRUE(dummyLookup == &dummy);

    const auto& sref = s;
    auto dummyConstLookup = sref.get_section(name1);
    EXPECT_TRUE(dummyConstLookup == &dummy);

    EXPECT_TRUE(s.is_root());
    EXPECT_FALSE(dummy.is_root());

    EXPECT_TRUE(dummy.parent() == &s);
  }

  TEST(conf, t_option_add_get)
  {
    section s{ "glob"sv };

    constexpr auto optName = "neko"sv;
    constexpr auto intVal = 10;
    constexpr auto boolVal = false;
    constexpr auto floatVal = 42.69f;
    constexpr auto strVal = "kitty"sv;

    auto&& opt = s.add_option(optName);
    EXPECT_TRUE(s.has_option(optName));

    opt.add_value(intVal);
    opt.add_value(boolVal);
    opt.add_value(floatVal);
    opt.add_value(strVal);

    const auto& optAdd = s.add_option(optName);
    ASSERT_EQ(optAdd.size(), 4);

    auto ip = optAdd[0].try_get<value::int_val>();
    ASSERT_TRUE(ip);
    EXPECT_EQ(*ip, intVal);

    auto bp = optAdd[1].try_get<value::bool_val>();
    ASSERT_TRUE(bp);
    EXPECT_EQ(*bp, boolVal);

    auto fp = optAdd[2].try_get<value::float_val>();
    ASSERT_TRUE(fp);
    EXPECT_DOUBLE_EQ(*fp, floatVal);

    auto sp = optAdd[3].try_get<value::str_val>();
    ASSERT_TRUE(sp);
    EXPECT_EQ(*sp, strVal);

    EXPECT_EQ((opt[0].get<value::int_val>()), intVal);
    EXPECT_EQ((opt[1].get<value::bool_val>()), boolVal);
    EXPECT_DOUBLE_EQ((opt[2].get<value::float_val>()), floatVal);
    EXPECT_EQ((opt[3].get<value::str_val>()), strVal);
  }

  TEST(conf, t_opt_base)
  {
    section s{ "empty"sv };

    option opt{ "opt"sv, s };
    opt.add_value(1);
    opt.add_value(2.0f);
    opt.add_value(false);
    opt.add_value("hi there"sv);

    using opt_struct = opt_tuple<value::int_val,
                                 value::float_val,
                                 value::bool_val,
                                 value::str_val>;

    auto toStruct = opt.to<opt_struct>();
    ASSERT_TRUE(toStruct);
    EXPECT_EQ((toStruct->get<0>()), 1ll);
    EXPECT_DOUBLE_EQ((toStruct->get<1>()), 2.0f);
    EXPECT_EQ((toStruct->get<2>()), false);
    EXPECT_EQ((toStruct->get<3>()), "hi there"sv);

    auto toTuple = opt.to<long long, float, bool, std::string_view>();
    ASSERT_TRUE(toTuple);
    EXPECT_EQ((std::get<0>(*toTuple)), 1ll);
    EXPECT_DOUBLE_EQ((std::get<1>(*toTuple)), 2.0f);
    EXPECT_EQ((std::get<2>(*toTuple)), false);
    EXPECT_EQ((std::get<3>(*toTuple)), "hi there"sv);
  }

  TEST(conf, t_opt_custom)
  {
    section s{ "empty"sv };

    option opt{ "opt"sv, s };
    opt.add_value(1);
    opt.add_value(2.0f);
    opt.add_value(false);
    opt.add_value("hi there"sv);

    class wrapper : public opt_wrapper<value::int_val,
                                       value::float_val,
                                       value::bool_val,
                                       value::str_val>
    {
    public:
      CLASS_SPECIALS_NODEFAULT(wrapper);

      wrapper(const option& opt) noexcept :
        opt_wrapper{ opt }
      { }

      auto& i() const noexcept
      {
        return *get<0>();
      }
      auto& f() const noexcept
      {
        return *get<1>();
      }
      auto& b() const noexcept
      {
        return *get<2>();
      }
      auto& s() const noexcept
      {
        return *get<3>();
      }
    };

    wrapper wrap{ opt };
    ASSERT_TRUE(wrap);

    EXPECT_EQ((wrap.i()), 1ll);
    EXPECT_DOUBLE_EQ((wrap.f()), 2.0f);
    EXPECT_EQ((wrap.b()), false);
    EXPECT_EQ((wrap.s()), "hi there"sv);
  }

  TEST(conf, t_single)
  {
    section s{ "empty"sv };

    option optI{ "optI"sv, s };
    optI.add_value(42);
    option optF{ "optF"sv, s };
    optF.add_value(69.0f);
    option optB{ "optB"sv, s };
    optB.add_value(true);
    option optS{ "optS"sv, s };
    optS.add_value("dummy"sv);

    int_opt io{ optI };
    ASSERT_TRUE(io);
    EXPECT_EQ(io.value(), 42);

    float_opt fo{ optF };
    ASSERT_TRUE(fo);
    EXPECT_FLOAT_EQ(fo.value(), 69.0f);

    bool_opt bo{ optB };
    ASSERT_TRUE(bo);
    EXPECT_TRUE(bo.value());

    string_opt so{ optS };
    ASSERT_TRUE(so);
    EXPECT_EQ(so.value(), "dummy"sv);

    option optFail1{ "fail1"sv, s };
    int_opt fi1{ optFail1 };
    ASSERT_FALSE(fi1);

    option optFail2{ "fail2"sv, s };
    optFail2.add_value(42);
    optFail2.add_value(69);
    int_opt fi2{ optFail2 };
    ASSERT_FALSE(fi2);
  }

  TEST(conf_file, t_bad)
  {
    constexpr auto fname = "derp.txt"sv;

    cfg_file f{ fname };
    EXPECT_FALSE(f);
    EXPECT_TRUE(f.line().empty());
  }

  TEST(conf_file, t_good)
  {
    constexpr auto fname = "tests/cfg/section.txt"sv;

    /****************************************
    *
    *      |.section
    *      |  option{ op1 }
    *      |  option{ op2, 1, true }
    *      |
    *      |  option{ after_empty }
    *
    *****************************************/

    cfg_file f{ fname };
    ASSERT_TRUE(f);

    auto line = f.line();
    EXPECT_EQ(line, ".section");
    line = f.line();
    EXPECT_EQ(line, "option{ op1 }");
    line = f.line();
    EXPECT_EQ(line, "option{ op2, 1, true }");
    line = f.line();
    EXPECT_EQ(line, "option{ after_empty }");

    EXPECT_FALSE(f);
  }

  TEST(conf_lex, t_good)
  {
    constexpr auto fname = "tests/cfg/lex_good.txt"sv;

    // .section { option1 { 1, -42, +6.9, 'string value', true, false } }

    cfg_file f{ fname };
    ASSERT_TRUE(f);

    lex l{ f };

    using detail::check_token;

    check_token(l, ".section"sv, token::section);
    check_token(l, "{"sv, token::curlyOpen);
    check_token(l, "option1"sv, token::identifier);
    check_token(l, "{"sv, token::curlyOpen);
    check_token(l, "1"sv, token::intNum);
    check_token(l, ","sv, token::comma);
    check_token(l, "-42"sv, token::intNum);
    check_token(l, ","sv, token::comma);
    check_token(l, "+6.9"sv, token::floatNum);
    check_token(l, ","sv, token::comma);
    check_token(l, "'string value'"sv, token::str);
    check_token(l, ","sv, token::comma);
    check_token(l, "true"sv, token::boolTrue);
    check_token(l, ","sv, token::comma);
    check_token(l, "false"sv, token::boolFalse);
    check_token(l, "}"sv, token::curlyClose);
    check_token(l, "}"sv, token::curlyClose);

    EXPECT_TRUE(!l);
  }

  TEST(conf_lex, t_bad1)
  {
    constexpr auto fname = "tests/cfg/lex_bad1.txt"sv;

    cfg_file f{ fname };
    ASSERT_TRUE(f);

    lex l{ f };

    using detail::check_token;

    check_token(l, {}, token::unknown);
    EXPECT_TRUE(!l);
  }

  TEST(conf_lex, t_bad2)
  {
    constexpr auto fname = "tests/cfg/lex_bad2.txt"sv;

    cfg_file f{ fname };
    ASSERT_TRUE(f);

    lex l{ f };

    using detail::check_token;

    check_token(l, ".section"sv, token::section);
    check_token(l, {}, token::unknown);
    EXPECT_TRUE(!l);
  }

  TEST(conf_lex, t_bad3)
  {
    constexpr auto fname = "tests/cfg/lex_bad3.txt"sv;

    cfg_file f{ fname };
    ASSERT_TRUE(f);

    lex l{ f };

    using detail::check_token;

    check_token(l, ".section"sv, token::section);
    check_token(l, "{"sv, token::curlyOpen);
    check_token(l, "option"sv, token::identifier);
    check_token(l, "{"sv, token::curlyOpen);
    check_token(l, "1"sv, token::intNum);
    check_token(l, ","sv, token::comma);
    check_token(l, {}, token::unknown);
    EXPECT_TRUE(!l);
  }

  TEST(conf_parser, t_opts)
  {
    constexpr auto fname = "tests/cfg/parse_opt.txt"sv;

    cfg c{ fname };
    ASSERT_TRUE(c);

    //.section
    auto sec = c->get_section("section"sv);
    ASSERT_TRUE(sec);

    const option* opt{};

    using detail::get_option;
    using detail::check_opt_value;

    //intp{ 42 }
    opt = get_option(*sec, "intp"sv);
    check_opt_value(*opt, 1, 0, 42ll);

    //intn{ -42 }
    opt = get_option(*sec, "intn"sv);
    check_opt_value(*opt, 1, 0, -42ll);

    //floatp{ 69.5 }
    opt = get_option(*sec, "floatp"sv);
    check_opt_value(*opt, 1, 0, 69.5f);

    //floatn{ -69.5 }
    opt = get_option(*sec, "floatn"sv);
    check_opt_value(*opt, 1, 0, -69.5f);

    //boolt{ true }
    opt = get_option(*sec, "boolt"sv);
    check_opt_value(*opt, 1, 0, true);

    //boolf{ false }
    opt = get_option(*sec, "boolf"sv);
    check_opt_value(*opt, 1, 0, false);

    //str{ 'hi there' }
    opt = get_option(*sec, "str"sv);
    check_opt_value(*opt, 1, 0, "hi there"sv);

    //mult{ 1, 2.0, 'three', true }
    opt = get_option(*sec, "mult"sv);
    check_opt_value(*opt, 4, 0, 1ll);
    check_opt_value(*opt, 4, 1, 2.0f);
    check_opt_value(*opt, 4, 2, "three"sv);
    check_opt_value(*opt, 4, 3, true);
  }

  TEST(conf_parser, t_subsec)
  {
    constexpr auto fname = "tests/cfg/parse_subsec.txt"sv;

    cfg c{ fname };
    ASSERT_TRUE(c);

    using detail::get_option;
    using detail::check_opt_value;

    //global{ 0 }
    auto opt = get_option(*c, "global"sv);
    check_opt_value(*opt, 1, 0, 0ll);

    constexpr auto localName = "local"sv;

    //.section
    auto sec = c->get_section("section"sv);
    ASSERT_TRUE(sec);

    //local{ 1 }
    opt = get_option(*sec, localName);
    check_opt_value(*opt, 1, 0, 1ll);

    //.subsection
    sec = sec->get_section("subsection"sv);
    ASSERT_TRUE(sec);

    //local{ 2 }
    opt = get_option(*sec, localName);
    check_opt_value(*opt, 1, 0, 2ll);

    //.inner
    sec = sec->get_section("inner"sv);
    ASSERT_TRUE(sec);

    //local{ 3 }
    opt = get_option(*sec, localName);
    check_opt_value(*opt, 1, 0, 3ll);

    //.another
    sec = c->get_section("another"sv);
    ASSERT_TRUE(sec);

    //local{ 4 }
    opt = get_option(*sec, localName);
    check_opt_value(*opt, 1, 0, 4ll);
  }

  TEST(conf_parser, t_bad1)
  {
    constexpr auto fname = "tests/cfg/parse_bad1.txt";

    cfg c{ fname };
    ASSERT_FALSE(c);
  }

  TEST(conf_parser, t_bad2)
  {
    constexpr auto fname = "tests/cfg/parse_bad2.txt";

    cfg c{ fname };
    ASSERT_FALSE(c);
  }

  TEST(conf_parser, t_bad3)
  {
    constexpr auto fname = "tests/cfg/parse_bad3.txt";

    cfg c{ fname };
    ASSERT_FALSE(c);
  }
}