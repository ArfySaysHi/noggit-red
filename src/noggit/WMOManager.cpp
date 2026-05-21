#include <Listfile.hpp>
#include <noggit/ContextObject.hpp>
#include <noggit/WMO.h>
#include <noggit/WMOManager.hpp>
#include <string>

decltype(WMOManager::_) WMOManager::_;

void WMOManager::report() {
  std::string output = "Still in the WMO manager:\n";
  _.apply([&](BlizzardArchive::Listfile::FileKey const &key, WMO const &) {
    output += " - " + key.stringRepr() + "\n";
  });
  LogDebug << output;
}

void WMOManager::clear_hidden_wmos() {
  _.apply([&](BlizzardArchive::Listfile::FileKey const &, WMO &wmo) {
    wmo.show();
  });
}

void WMOManager::unload_all(Noggit::NoggitRenderContext context) {
  _.context_aware_apply([&](BlizzardArchive::Listfile::FileKey const &,
                            WMO &wmo) { wmo.renderer()->unload(); },
                        context);
}

ScopedWMOReference::ScopedWMOReference(
    BlizzardArchive::Listfile::FileKey const &file_key,
    Noggit::NoggitRenderContext context)
    : _valid(true), _file_key(file_key), _context(context),
      _wmo(WMOManager::_.emplace(file_key, context)) {}

ScopedWMOReference::ScopedWMOReference(ScopedWMOReference const &other)
    : _valid(other._valid), _file_key(other._file_key),
      _wmo(WMOManager::_.emplace(_file_key, other._context)),
      _context(other._context) {}

ScopedWMOReference &
ScopedWMOReference::operator=(ScopedWMOReference const &other) {
  if (this != &other) {
    _valid = other._valid;
    _file_key = other._file_key;
    _wmo = WMOManager::_.emplace(_file_key, other._context);
    _context = other._context;
  }
  return *this;
}

ScopedWMOReference::ScopedWMOReference(ScopedWMOReference &&other)
    : _valid(other._valid), _file_key(other._file_key), _wmo(other._wmo),
      _context(other._context) {
  other._valid = false;
}

ScopedWMOReference &ScopedWMOReference::operator=(ScopedWMOReference &&other) {
  if (this != &other) {
    std::swap(_valid, other._valid);
    std::swap(_file_key, other._file_key);
    std::swap(_wmo, other._wmo);
    std::swap(_context, other._context);
    other._valid = false;
  }
  return *this;
}

ScopedWMOReference::~ScopedWMOReference() {
  if (_valid) {
    WMOManager::_.erase(_file_key, _context);
  }
}

WMO *ScopedWMOReference::operator->() const { return _wmo; }

WMO *ScopedWMOReference::get() const { return _wmo; }
