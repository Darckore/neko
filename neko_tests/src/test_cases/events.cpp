#include "managers/event.hpp"
using neko::event;
using neko::event_subscriber;

namespace neko_tests
{
  namespace detail
  {
    struct ev1
    {
      int value{};
    };

    struct ev1_src
    {
      using ev = event<ev1>;
      ev1_src() = default;

      void produce() noexcept
      {
        ev::push(cnt++);
      }
      void dispatch() noexcept
      {
        ev::dispatch();
      }

      int cnt = 0;
    };
  }

  TEST(evt, t_sub_unsub)
  {
    using ev = event<detail::ev1>;
    constexpr auto maxEvents = 10u;
    std::vector<int> values;
    values.reserve(maxEvents);
    void* consumer = &values; // Safe because the consumer ptr is only used as an id

    {
      event_subscriber<detail::ev1> sub{ consumer,
        [&values](const auto& e)
        {
          values.push_back(e.value);
        }
      };

      ASSERT_EQ(ev::sub_count(),     1u);
      ASSERT_EQ(ev::pending_count(), 0u);

      detail::ev1_src ev_src;
      for (auto i = 0u; i < maxEvents; ++i)
      {
        ev_src.produce();
      }

      EXPECT_EQ(ev::pending_count(), maxEvents);
      ev_src.dispatch();
      EXPECT_EQ(ev::pending_count(), 0u);

      for (auto i = 0; i < maxEvents; ++i)
      {
        EXPECT_EQ(values[i], i);
      }

      ev_src.produce();
      ev_src.produce();
      ev_src.produce();
      EXPECT_EQ(ev::pending_count(), 3u);
    }

    ASSERT_EQ(ev::sub_count(),     0u);
    ASSERT_EQ(ev::pending_count(), 0u);
  }

  namespace detail
  {
    struct ev2
    {
      int one{};
      int two{};
    };

    struct ev3
    {
      float f{};
    };

    struct ev23_src
    {
      using e1 = event<ev2>;
      using e2 = event<ev3>;
      ev23_src() = default;

      void produce(int i) noexcept
      {
        e2::push(static_cast<float>(i));

        if (i % 2 == 0)
        {
          e1::push(i, i + 1);
        }
      }
      void dispatch() noexcept
      {
        e1::dispatch();
        e2::dispatch();
      }
    };

    struct ev23_consumer
    {
      using e1 = detail::ev2;
      using e2 = detail::ev3;
      ev23_consumer()
      {
        es1 = { this, [this](const auto& e){ on_e1(e); }};
        es2 = { this, [this](const auto& e){ on_e2(e); }};
      }

      event_subscriber<e1> es1;
      event_subscriber<e2> es2;
      int cnt1 = 0;
      int cnt2 = 0;

      void on_e1(const e1& e) noexcept
      {
        EXPECT_EQ(e.one, cnt1);
        EXPECT_EQ(e.two, cnt1 + 1);
        cnt1 += 2;
      }
      void on_e2(const e2& e) noexcept
      {
        EXPECT_FLOAT_EQ(e.f, static_cast<float>(cnt2));
        ++cnt2;
      }
    };
  }

  TEST(evt, t_many_one)
  {
    constexpr auto maxEvents = 10u;

    detail::ev23_src src;
    detail::ev23_consumer cons;
    for (auto i = 0; i < maxEvents; ++i)
    {
      src.produce(i);
    }

    using e1 = event<detail::ev2>;
    using e2 = event<detail::ev3>;
    ASSERT_EQ(e1::sub_count(), 1u);
    ASSERT_EQ(e2::sub_count(), 1u);
    ASSERT_EQ(e1::pending_count(), maxEvents / 2u);
    ASSERT_EQ(e2::pending_count(), maxEvents);

    src.dispatch();
  }
}