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

  const conf_manager::cfg_type* conf_manager::operator[](key_type key) const noexcept
  {
    return lookup(key);
  }

  // Public members:

  bool conf_manager::load_file(key_type key, path_type fname) noexcept
  {
    fname = canonise(std::move(fname));
    if (fname.empty())
    {
      return false;
    }

    NEK_TRACE("Opening config file {}", fname.string());
    auto foundKey = lookup_index(fname);
    if (foundKey)
    {
      if (*foundKey != key)
      {
        logger::warning("Path {} is already mapped to a different name", fname.string());
        return false; 
      }

      return true;
    }

    auto&& [newItem, ok] = m_storage.emplace(key, cfg_type{fname});
    if (!ok || !static_cast<bool>(newItem->second))
    {
      logger::error("Unable to open file {}", fname.string());
      return false;
    }

    m_index.emplace(std::move(fname), key);
    return true;
  }

  bool conf_manager::exists(key_type key) const noexcept
  {
    return static_cast<bool>(lookup(key));
  }

  const conf_manager::cfg_type* conf_manager::lookup(key_type key) const noexcept
  {
    auto item = m_storage.find(key);
    return item != m_storage.end() ? &item->second : nullptr;
  }

  // Private members

  conf_manager::key_opt conf_manager::lookup_index(const path_type& path) const noexcept
  {
    auto item = m_index.find(path);
    if (item == m_index.end())
    {
      return {};
    }

    return item->second;
  }

  path_type conf_manager::canonise(path_type path) const noexcept
  {
    auto fullPath = (path.is_absolute()) ? path : m_root / path;

    std::error_code err;
    path = fsys::canonical(fullPath, err);
    if (err)
    {
      logger::warning("Unable to open config file {}. {}", fullPath.string(), err.message());
    }
    
    return path;
  }
}