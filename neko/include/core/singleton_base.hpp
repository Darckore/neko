//
// Base of all systems
//

#pragma once

namespace neko
{
  //
  // The base class for all systems in the engine
  // Ensures a single instance, manages own lifetime
  //
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
    //
    // The instance pointer
    //
    inline static pointer instance;

  public:
    //
    // Checks whether the instance exists
    //
    static bool good() noexcept
    {
      return static_cast<bool>(instance);
    }

    //
    // Returns an instance reference
    //
    static Derived& get() noexcept
    {
      NEK_ASSERT(good());
      return *instance;
    }

    //
    // Destroys the instance
    //
    static void shutdown() noexcept
    {
      instance.reset(nullptr);
    }

    //
    // Creates an instance
    // Parameters passed to this function must match the actual class's
    // constructor signature
    //
    template <typename Sys, typename ...Args>
    static bool create(Args&& ...args) noexcept
    {
      if (!instance)
      {
        instance.reset(new (std::nothrow) Sys{ std::forward<Args>(args)... });
      }

      return good();
    }
  };

}