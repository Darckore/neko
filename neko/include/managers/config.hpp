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
    CLASS_SPECIALS_NONE_CUSTOM(conf_manager);

    ~conf_manager() noexcept;

  private:
    conf_manager() noexcept;

  public:

  private:
    store_type m_storage;
    path_type m_root;
  };
}