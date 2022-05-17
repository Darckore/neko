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

  class pipeline
  {
  public:
    CLASS_SPECIALS_NONE_CUSTOM(pipeline);
    
    virtual ~pipeline() noexcept = default;

  protected:
    pipeline() noexcept = default;

  private:
    friend class renderer;

    template <detail::pipeline_imp Pipeline>
    static auto create(const host_info& info) noexcept
    {
      return pipeline_ptr{ new (std::nothrow) Pipeline{ info } };
    }

  public:
    virtual bool good() noexcept = 0;
  };
}