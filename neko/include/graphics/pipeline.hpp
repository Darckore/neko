//
// Platform pipeline
//

#pragma once

namespace neko::platform
{
  struct host_info;
  class pipeline;

  namespace detail
  {
    template <typename T>
    concept pipeline_imp = std::is_base_of_v<pipeline, T> && std::is_constructible_v<T, const host_info&>;
  }

  using pipeline_ptr = neko::pointer<pipeline>;

  //
  // Interface for specific platform-dependent
  // pipeline implementations (DirectX, Vulkan, etc.)
  //
  class pipeline
  {
  public:
    CLASS_SPECIALS_NONE_CUSTOM(pipeline);
    
    virtual ~pipeline() noexcept = default;

  protected:
    pipeline() noexcept = default;

  private:
    friend class renderer;

    //
    // Creates a new instance using app host information
    //
    template <detail::pipeline_imp Pipeline>
    static auto create(const host_info& info) noexcept
    {
      return pipeline_ptr{ new (std::nothrow) Pipeline{ info } };
    }

  public:
    //
    // Checks whether the pipeline is in valid state
    //
    virtual bool good() noexcept = 0;
  };
}