#pragma once
#include "core/singleton_base.hpp"

namespace neko
{
  namespace config
  {
    class cfg;
  }

  class conf_manager : private singleton<conf_manager>
  {
  private:
    friend class singleton<conf_manager>;
    using path_type  = fsys::path;
    using key_type   = std::string;
    using cfg_type   = config::cfg;
    using store_type = std::unordered_map<key_type, cfg_type>;

  public:
    CLASS_SPECIALS_NONE(conf_manager);

    ~conf_manager() noexcept;

  private:
    explicit conf_manager(const path_type& root) noexcept;

  public:
    bool load_file(path_type fname) noexcept;

  private:
    store_type m_storage;
    path_type m_root;
  };
}