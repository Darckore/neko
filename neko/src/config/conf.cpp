#include "config/conf.hpp"
#include "config/parser/parser.hpp"

namespace neko::config
{
  // Special members

  cfg::cfg(file_name fname) noexcept :
    m_file{ std::move(fname) },
    m_root{ parser::root_name }
  {
    read();
  }

  cfg::operator bool() const noexcept
  {
    return static_cast<bool>(m_file);
  }

  const section* cfg::operator->() const noexcept
  {
    return &m_root;
  }

  const section& cfg::operator*() const noexcept
  {
    return m_root;
  }

  // Private members

  void cfg::read() noexcept
  {
    if (!parse())
    {
      m_file.discard();
      return;
    }

    m_file.rewind();
  }

  bool cfg::parse() noexcept
  {
    if (parser p{ m_file })
    {
      m_root = p.get();
      return true;
    }
    return false;
  }
}