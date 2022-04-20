#pragma once

namespace neko
{
  template <typename Event>
  class event_handler
  {
  public:
    using consumer_handle  = std::uintptr_t;
    using consumer_pointer = const void*;
    using ref_type         = const Event&;
    using handler_type     = std::function<void(ref_type)>;

  private:
    static auto to_handle(consumer_pointer cptr) noexcept
    {
      return reinterpret_cast<consumer_handle>(cptr);
    }

  public:
    CLASS_SPECIALS_NODEFAULT(event_handler);

    event_handler(consumer_handle c, handler_type handler) noexcept :
      m_consumer{ c },
      m_handler{ std::move(handler) }
    {}

    event_handler(consumer_pointer cptr, handler_type handler) noexcept :
      event_handler(to_handle(cptr), std::move(handler))
    {}

    explicit operator bool() const noexcept
    {
      return static_cast<bool>(m_consumer);
    }

    bool operator==(const event_handler& other) const noexcept
    {
      return m_consumer == other.m_consumer;
    }
    bool operator==(consumer_handle c) const noexcept
    {
      return m_consumer == c;
    }
    bool operator==(consumer_pointer cptr) const noexcept
    {
      return *this == to_handle(cptr);
    }

  public:
    void operator()(ref_type evt) const noexcept
    {
      NEK_ASSERT(m_handler);
      if (m_handler)
      {
        m_handler(evt);
      }
    }

  private:
    consumer_handle m_consumer{};
    handler_type    m_handler{};
  };


  template <typename Event>
  class event
  {
  private:
    using event_queue     = std::queue<Event>;
    using handler_type    = event_handler<Event>;
    using handler_raw     = handler_type::handler_type;
    using consumer_handle = handler_type::consumer_handle;
    using consumer_ptr    = handler_type::consumer_pointer;
    using sub_list        = std::vector<handler_type>;

  public:
    CLASS_SPECIALS_ALL_CUSTOM(event);

  private:
    event() noexcept = default;

  public:
    static bool subscribe(handler_type handler) noexcept
    {
      if (!handler)
      {
        return false;
      }

      if (auto it = std::find(m_subs.begin(), m_subs.end(), handler); it != m_subs.end())
      {
        NEK_TRACE("An event consumer tried to subscribe more than once");
        NEK_ASSERT(false);
        return false;
      }

      m_subs.push_back(std::move(handler));
      return true;
    }

    static bool subscribe(consumer_ptr c, handler_raw fn) noexcept
    {
      return subscribe(handler_type{ c, std::move(fn) });
    }

    static void unsubscribe(consumer_ptr c) noexcept
    {
      if (!c)
      {
        return;
      }

      auto it = std::remove_if(m_subs.begin(), m_subs.end(),
                [c](const auto& handler)
                {
                  return handler == c;
                });

      m_subs.erase(it, m_subs.end());
      
      if (m_subs.empty())
      {
        m_queue = {};
      }
    }

    template <typename ...Args>
    static void push(Args&& ...args) noexcept
    {
      if (!m_subs.empty())
      {
        m_queue.emplace(std::forward<Args>(args)...);
      }
    }

    static void dispatch() noexcept
    {
      while (!m_queue.empty())
      {
        dispatch_one();
      }
    }

    static auto sub_count() noexcept
    {
      return m_subs.size();
    }

    static auto pending_count() noexcept
    {
      return m_queue.size() * m_subs.size();
    }

  private:
    static void dispatch_one() noexcept
    {
      NEK_ASSERT(!m_queue.empty());
      for (auto&& evt = m_queue.front();
           auto&& handler : m_subs)
      {
        handler(evt);
      }

      m_queue.pop();
    }

  private:
    inline static sub_list m_subs;
    inline static event_queue m_queue;
  };


  template <typename Event>
  class event_subscriber
  {
  public:
    using event_type   = event<Event>;
    using handler_type = event_handler<Event>;
    using handler_raw  = handler_type::handler_type;
    using consumer_ptr = handler_type::consumer_pointer;

  public:
    event_subscriber() = default;

    event_subscriber(const event_subscriber&) = delete;
    event_subscriber& operator=(const event_subscriber&) = delete;

    event_subscriber(event_subscriber&& other) noexcept : 
      m_consumer{ other.m_consumer }
    {
      other.m_consumer = 0;
    }
    event_subscriber& operator=(event_subscriber&& other) noexcept
    {
      if (this != &other)
      {
        m_consumer = other.m_consumer;
        other.m_consumer = 0;
      }
      return *this;
    }

    ~event_subscriber() noexcept
    {
      if (m_consumer)
      {
        event_type::unsubscribe(m_consumer);
      }
    }

    event_subscriber(consumer_ptr c, handler_raw handler) noexcept :
      m_consumer{ c }
    {
      event_type::subscribe(c, std::move(handler));
    }

    explicit operator bool() const noexcept
    {
      return static_cast<bool>(m_consumer);
    }

    void reset() noexcept
    {
      m_consumer = consumer_ptr{};
    }

  private:
    consumer_ptr m_consumer{};
  };

}