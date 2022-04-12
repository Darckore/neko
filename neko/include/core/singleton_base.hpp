#pragma once

namespace neko
{
  template <typename Derived>
  class singleton
  {
  public:
    using pointer = std::unique_ptr<Derived>;

  public:
    CLASS_SPECIALS_NONE_CUSTOM(singleton);
    
  protected:
    singleton() = default;

  protected:
    static pointer& instance() noexcept
    {
      static pointer ptr;
      return ptr;
    }

  public:
    static Derived& get() noexcept
    {
      return *instance();
    }

    static void shutdown() noexcept
    {
      instance().reset(nullptr);
    }

    template <typename ...Args>
    static bool create(Args&& ...args) noexcept
    {
      auto&& inst = instance();
      if (!inst)
      {
        inst.reset(new (std::nothrow) Derived{ std::forward<Args>(args)... });
      }

      return static_cast<bool>(inst);
    }
  };

}