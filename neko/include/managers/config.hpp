#pragma once
#include "core/singleton_base.hpp"

namespace neko
{
  namespace config
  {
    class cfg;
  }

  class conf_manager final : private singleton<conf_manager>
  {
  private:
    friend class singleton<conf_manager>;

    using key_type   = hashed_string;
    using cfg_type   = config::cfg;
    using store_type = std::unordered_map<key_type, cfg_type>;
    using index_type = std::unordered_map < path_type, key_type, path_hasher> ;
    
    template <typename T>
    using res_opt = std::optional<T>;

    using key_opt = res_opt<key_type>;

  public:
    CLASS_SPECIALS_NONE(conf_manager);

    ~conf_manager() noexcept;

    const cfg_type* operator[](key_type key) const noexcept;

  private:
    explicit conf_manager(const path_type& root) noexcept;

  public:
    bool load_file(key_type key, path_type fname) noexcept;

    bool exists(key_type key) const noexcept;
    const cfg_type* lookup(key_type key) const noexcept;

  private:
    key_opt lookup_index(const path_type& path) const noexcept;
    path_type canonise(path_type path) const noexcept;

  private:
    store_type m_storage;
    index_type m_index;
    path_type m_root;
  };
}