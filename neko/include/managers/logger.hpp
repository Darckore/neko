#pragma once

namespace neko
{
  class logger final
  {
  public:
    enum class level : std::uint8_t
    {
      off,
      err,
      warn,
      msg,
      dbg
    };

    using enum level;

    using fmt_type  = std::string_view;
    using buf_type  = std::string;
    using file_name = fsys::path;

  public:
    CLASS_SPECIALS_NONE(logger);

  private:
    static void prologue(level lvl)
    {
      constexpr std::array severities {
        "[error]"sv,
        "[warn] "sv,
        "[note] "sv,
        "[trace]"sv
      };

      constexpr auto fmt = "=={1:%F, %H:%M:%OS}== {0:}: "sv;
      const auto idx = static_cast<std::size_t>(lvl) - 1;
      const auto zt = std::chrono::zoned_time{
        std::chrono::current_zone(),
        std::chrono::system_clock::now()
      };

      std::format_to(std::back_inserter(m_buf), fmt,
                     severities[idx],
                     zt);
    }

    template <typename ...Args>
    static void message(level lvl, fmt_type fmt, Args&& ...args) noexcept
    {
      if (lvl > m_lvl)
      {
        return;
      }

      try
      {
        prologue(lvl);
        std::format_to(std::back_inserter(m_buf), fmt, std::forward<Args>(args)...);
      }
      catch (std::format_error& e)
      {
        m_buf.clear();
        std::format_to(std::back_inserter(m_buf), "Bad format string: {}", e.what());
      }
      catch (std::runtime_error& e)
      {
        m_buf.clear();
        std::format_to(std::back_inserter(m_buf), "Runtime error: {}", e.what());
      }
      catch (std::bad_alloc& e)
      {
        m_buf.clear();
        abnormal(e.what());
        return;
      }
      
      m_stream  << m_buf << '\n';
      std::cout << m_buf << '\n';

      m_buf.clear();
    }

  public:
    static bool good() noexcept
    {
      return static_cast<bool>(m_file);
    }

    static void init() noexcept
    {
      constexpr auto initialSize = 256ull;
      m_buf.reserve(initialSize);

      m_file.open(m_fname.c_str());
      NEK_ASSERT(good());
    }

    static void dump() noexcept
    {
      NEK_ASSERT(static_cast<bool>(m_file));
      m_file << m_stream.rdbuf();
      m_stream = {};
    }

    static void shutdown() noexcept
    {
      dump();
      m_file.close();
    }

    static level set_severity_level(level lvl) noexcept
    {
      const auto prev = m_lvl;
      m_lvl = lvl;
      return prev;
    }

    static void assign_file(file_name fname) noexcept
    {
      m_fname = std::move(fname);
      m_file.open(m_fname);
      NEK_ASSERT(good());
    }

  #ifndef NDEBUG
    template <typename ...Args>
    static void trace(fmt_type fmt, Args&& ...args) noexcept
    {
      message(dbg, fmt, std::forward<Args>(args)...);
    }
  #endif

    template <typename ...Args>
    static void note(fmt_type fmt, Args&& ...args) noexcept
    {
      message(msg, fmt, std::forward<Args>(args)...);
    }

    template <typename ...Args>
    static void warning(fmt_type fmt, Args&& ...args) noexcept
    {
      message(warn, fmt, std::forward<Args>(args)...);
    }

    template <typename ...Args>
    static void error(fmt_type fmt, Args&& ...args) noexcept
    {
      message(err, fmt, std::forward<Args>(args)...);
    }

    static void abnormal(std::string_view msg) noexcept
    {
      NEK_ASSERT(good());
      dump();
      m_file << "Abnormal termination. " << msg << '\n';
    }

  private:
    inline static file_name m_fname{ "engine.log" };
    inline static buf_type  m_buf;

    inline static std::stringstream m_stream;
    inline static std::ofstream m_file;

    inline static level m_lvl{
    #ifndef NDEBUG
      dbg
    #else
      err
    #endif
    };
  };
}