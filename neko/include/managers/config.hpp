//
// Config manager
//

#pragma once
#include "core/singleton_base.hpp"

namespace neko
{
  namespace config
  {
    class cfg;
  }

  //
  // Configuration manager
  // Maintains a dictionary of all active configuration files
  //
  class conf_manager final : private singleton<conf_manager>
  {
  private:
    friend class singleton<conf_manager>;

    //
    // Key into config map
    //
    using key_type   = hashed_string;

    //
    // Container for a config file
    //
    using cfg_type   = config::cfg;

    //
    // Storage type for config containers
    //
    using store_type = std::unordered_map<key_type, cfg_type>;

    //
    // Store index type
    //
    using index_type = std::unordered_map < path_type, key_type, path_hasher> ;
    
    //
    // Helper for an optional value
    //
    template <typename T>
    using res_opt = std::optional<T>;

    //
    // Optional key value
    //
    using key_opt = res_opt<key_type>;

  public:
    CLASS_SPECIALS_NONE(conf_manager);

    ~conf_manager() noexcept;

    //
    // Looks up the specified config container by associated name
    //
    const cfg_type* operator[](key_type key) const noexcept;

  private:
    //
    // Constructs the manager from a path to the root DIRECTORY
    // which is used as a base path for config files
    //
    explicit conf_manager(const path_type& root) noexcept;

  public:
    //
    // Tries to load a file by a path relative to the root directory
    // and associates a name with it
    // Returns false if such element already exists
    //
    bool load_file(key_type key, path_type fname) noexcept;

    //
    // Checks if the specified name is already used
    //
    bool exists(key_type key) const noexcept;

    //
    // Same as []
    //
    const cfg_type* lookup(key_type key) const noexcept;

  private:
    //
    // Searches the index to find out whether a path is already in use
    // Returns an optional key value of the found item
    //
    key_opt lookup_index(const path_type& path) const noexcept;

    //
    // Takes a relative path to a config file and canonises it
    //
    path_type canonise(path_type path) const noexcept;

  private:
    //
    // Config container storage
    //
    store_type m_storage;

    //
    // Store index
    //
    index_type m_index;

    //
    // Root directory path
    //
    path_type m_root;
  };
}