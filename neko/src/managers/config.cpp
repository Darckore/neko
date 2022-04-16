#include "managers/config.hpp"
#include "config/conf.hpp"

namespace neko
{
  // Special members

  conf_manager::~conf_manager() noexcept = default;

  conf_manager::conf_manager(const path_type& root) noexcept
  {
    std::error_code err;
    m_root = fsys::absolute(root, err);
    if (err)
    {
      logger::error("Path {} is invalid. {}", root.string(), err.message());
      return;
    }

    logger::note("Root path for configuration files: {}", m_root.string());
  }

  // Public members:

  bool conf_manager::load_file(path_type fname) noexcept
  {
    utils::unused(fname);
    return true;
  }
}