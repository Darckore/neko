//
// Event manager
//

#pragma once

namespace neko
{
  //
  // Event handler wrapper
  // Used to register a consumer callback with an event
  //
  template <typename Event>
  class event_handler
  {
  public:
    //
    // An integer handle of the consumer
    //
    using consumer_handle  = std::uintptr_t;

    //
    // A generic consumer pointer
    //
    using consumer_pointer = const void*;

    //
    // Reference to the underlying event data structure
    //
    using ref_type         = const Event&;

    //
    // Consumer callback
    //
    using handler_type     = std::function<void(ref_type)>;

  private:
    //
    // Casts consumer pointer to integer representation
    //
    static auto to_handle(consumer_pointer cptr) noexcept
    {
      return reinterpret_cast<consumer_handle>(cptr);
    }

  public:
    CLASS_SPECIALS_NODEFAULT(event_handler);

    //
    // Constructs a handler from an integer handle and a callback function
    //
    event_handler(consumer_handle c, handler_type handler) noexcept :
      m_consumer{ c },
      m_handler{ std::move(handler) }
    {}

    //
    // Constructs a handler from a consumer pointer and a callback function
    //
    event_handler(consumer_pointer cptr, handler_type handler) noexcept :
      event_handler(to_handle(cptr), std::move(handler))
    {}

    //
    // Checks whether the consumer is valid
    //
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
    //
    // Executes the callback function
    //
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


  //
  // Event dispatcher type
  // The template parameter is an underlying client-provided data structure
  // Typically, it should be a lightweight POD type containing some data
  // representing an event
  // 
  // A good example is the input map and the window communication
  // See raw_input.hpp for underlying data examples
  //
  template <typename Event>
  class event
  {
  private:
    //
    // Event queue type
    //
    using event_queue     = std::queue<Event>;

    //
    // Associated handler type
    //
    using handler_type    = event_handler<Event>;

    //
    // Callback function type from the handler
    //
    using handler_raw     = handler_type::handler_type;

    //
    // Event consumer handle
    //
    using consumer_handle = handler_type::consumer_handle;

    //
    // Event consumer pointer
    //
    using consumer_ptr    = handler_type::consumer_pointer;

    //
    // An array of subscribed handlers
    //
    using sub_list        = std::vector<handler_type>;

  public:
    CLASS_SPECIALS_ALL_CUSTOM(event);

  private:
    event() noexcept = default;

  public:
    //
    // Tries to subscribe a handler to this event
    // Returns false is the same consumer is already subscribed
    //
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

    //
    // Tries to subscribe a handler by consumer ptr and callback
    //
    static bool subscribe(consumer_ptr c, handler_raw fn) noexcept
    {
      return subscribe(handler_type{ c, std::move(fn) });
    }

    //
    // Unsubscribes a consumer
    // Consumers subsribing to events manually must explicitly call this
    // to stop subscribing to the event
    //
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

    //
    // Constructs an underlying data structure and pushes it to the queue
    //
    template <typename ...Args>
    static void push(Args&& ...args) noexcept
    {
      if (!m_subs.empty())
      {
        m_queue.emplace(std::forward<Args>(args)...);
      }
    }

    //
    // Dispatches all events to all consumers
    //
    static void dispatch() noexcept
    {
      while (!m_queue.empty())
      {
        dispatch_one();
      }
    }

    //
    // Returns the number of suscribers
    //
    static auto sub_count() noexcept
    {
      return m_subs.size();
    }

    //
    // Returns the total number of events waiting to be dispatched
    // (queue size * sub_count)
    //
    static auto pending_count() noexcept
    {
      return m_queue.size() * m_subs.size();
    }

  private:
    //
    // Dispatches one event to all subscribers
    //
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
    //
    // List of all current subscribers
    //
    inline static sub_list m_subs;

    //
    // Event queue
    //
    inline static event_queue m_queue;
  };


  //
  // RAII helper class managing a subsribtion of a single consumer
  // to a single event
  // 
  // It is somewhat similar to std::unique_ptr
  // Consumers can create this and hold it until they are destroyed
  // for automatic unsibscribing
  //
  template <typename Event>
  class event_subscriber
  {
  public:
    //
    // Associated event type
    //
    using event_type   = event<Event>;

    //
    // Associated handler type
    //
    using handler_type = event_handler<Event>;

    //
    // Callback function type from the handler
    //
    using handler_raw  = handler_type::handler_type;

    //
    // Event consumer pointer
    //
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

    //
    // Subscriber a consumer with the specified callback
    // The pointer is actually not used for anything,
    // it is basically just an index of sorts
    // 
    // Any non-zero value will do
    //
    event_subscriber(consumer_ptr c, handler_raw handler) noexcept :
      m_consumer{ c }
    {
      event_type::subscribe(c, std::move(handler));
    }

    //
    // Checks if the subscriber points to a valid consumer
    //
    explicit operator bool() const noexcept
    {
      return static_cast<bool>(m_consumer);
    }

    //
    // Resets into the invalid state
    //
    void reset() noexcept
    {
      m_consumer = consumer_ptr{};
    }

  private:
    consumer_ptr m_consumer{};
  };

}