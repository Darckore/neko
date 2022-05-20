//
// Engine logger
//

#pragma once

namespace neko
{
  //
  // Logger for the engine
  // Systems and user code can use it to post information to the application log
  //
  class logger final
  {
  public:
    //
    // Loggind level
    //
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
    //
    // Generates a time stamp and message type indicator
    //
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

    //
    // Posts a message of the specified level
    // The fmt parameter is a format string (see std::format)
    //
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
        std::vformat_to(std::back_inserter(m_buf), fmt, std::make_format_args(std::forward<Args>(args)...));
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

      if (m_stream.view().size() >= bufferSizeMax)
      {
        dump();
        init_stream();
      }
    }

    //
    // Reserves memory for the stream
    //
    static void init_stream() noexcept
    {
      std::string buf;
      buf.reserve(bufferSizeMax);
      m_stream.str(std::move(buf));
    }

  public:
    //
    // Checks the log file state
    //
    static bool good() noexcept
    {
      return static_cast<bool>(m_file);
    }

    //
    // Initialises the logger
    // Creates the underlying buffer and opens the log file for writing
    //
    static void init() noexcept
    {
      init_stream();

      constexpr auto initialSize = 256ull;
      m_buf.reserve(initialSize);

      m_file.open(m_fname.c_str());
      NEK_ASSERT(good());
    }

    //
    // Writes buffer contents to the file and clears the buffer
    //
    static void dump() noexcept
    {
      NEK_ASSERT(static_cast<bool>(m_file));
      m_file << m_stream.rdbuf();
      m_stream = {};
    }

    //
    // Writes data to the file and closes it
    //
    static void shutdown() noexcept
    {
      dump();
      m_file.close();
    }

    //
    // Sets the logging level
    // Returns the previously set one
    // Applications can use this to override the level temporarily
    //
    static level set_severity_level(level lvl) noexcept
    {
      const auto prev = m_lvl;
      m_lvl = lvl;
      return prev;
    }

    //
    // Assigns a new file to write to
    //
    static void assign_file(file_name fname) noexcept
    {
      if (good())
      {
        dump();
      }

      m_fname = std::move(fname);
      m_file.open(m_fname);
      NEK_ASSERT(good());
    }

  #ifndef NDEBUG
    //
    // Debug trace message of the lowest level possible
    //
    template <typename ...Args>
    static void trace(fmt_type fmt, Args&& ...args) noexcept
    {
      message(dbg, fmt, std::forward<Args>(args)...);
    }
  #endif

    //
    // A note (informational message)
    // Disabled in release builds by default
    //
    template <typename ...Args>
    static void note(fmt_type fmt, Args&& ...args) noexcept
    {
      message(msg, fmt, std::forward<Args>(args)...);
    }

    //
    // A warning (something to worry about, but the engine can continue running)
    // Disabled in release builds by default
    //
    template <typename ...Args>
    static void warning(fmt_type fmt, Args&& ...args) noexcept
    {
      message(warn, fmt, std::forward<Args>(args)...);
    }

    //
    // An error (fatal condition which leads to engine shutdown)
    // Always enabled
    //
    template <typename ...Args>
    static void error(fmt_type fmt, Args&& ...args) noexcept
    {
      message(err, fmt, std::forward<Args>(args)...);
    }

    //
    // An abnormal termination message
    // Posted when normal logging is impossible (e.g. in an out of memory situation)
    //
    static void abnormal(std::string_view msg) noexcept
    {
      NEK_ASSERT(good());
      dump();
      m_file << "Abnormal termination. " << msg << '\n';
    }

  private:
    //
    // We'll maintain buffer size of no more than 4Kb
    //
    static constexpr auto bufferSizeMax = 4096ull;

    //
    // Log file name
    //
    inline static file_name m_fname{ "engine.log" };

    //
    // Buffer for formatted messages
    //
    inline static buf_type  m_buf;

    //
    // Underlying log buffer where messages are accumulated
    //
    inline static std::stringstream m_stream;

    //
    // Log file currently written to
    //
    inline static std::ofstream m_file;

    //
    // Current logging level
    //
    inline static level m_lvl{
    #ifndef NDEBUG
      dbg
    #else
      msg
    #endif
    };
  };
}