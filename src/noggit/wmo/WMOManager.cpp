#include <Listfile.hpp>
#include <noggit/ContextObject.hpp>
#include <noggit/wmo/WMO.hpp>
#include <noggit/wmo/WMOManager.hpp>

decltype(Noggit::WMO::WMOManager::_) Noggit::WMO::WMOManager::_;

void Noggit::WMO::WMOManager::clear_hidden_wmos() {
  _.apply([&](BlizzardArchive::Listfile::FileKey const &, WMO &wmo) {
    wmo.show();
  });
}

void Noggit::WMO::WMOManager::unload_all(Noggit::NoggitRenderContext context) {
  _.context_aware_apply([&](BlizzardArchive::Listfile::FileKey const &,
                            WMO &wmo) { wmo.renderer()->unload(); },
                        context);
}

Noggit::WMO::ScopedWMOReference::ScopedWMOReference(
    BlizzardArchive::Listfile::FileKey const &file_key,
    Noggit::NoggitRenderContext context)
    : _valid(true), _file_key(file_key), _context(context),
      _wmo(WMOManager::_.emplace(file_key, context)) {}

Noggit::WMO::ScopedWMOReference::ScopedWMOReference(
    ScopedWMOReference const &other)
    : _valid(other._valid), _file_key(other._file_key),
      _wmo(WMOManager::_.emplace(_file_key, other._context)),
      _context(other._context) {}

Noggit::WMO::ScopedWMOReference &
Noggit::WMO::ScopedWMOReference::operator=(ScopedWMOReference const &other) {
  if (this != &other) {
    _valid = other._valid;
    _file_key = other._file_key;
    _wmo = WMOManager::_.emplace(_file_key, other._context);
    _context = other._context;
  }
  return *this;
}

Noggit::WMO::ScopedWMOReference::ScopedWMOReference(ScopedWMOReference &&other)
    : _valid(other._valid), _file_key(other._file_key), _wmo(other._wmo),
      _context(other._context) {
  other._valid = false;
}

Noggit::WMO::ScopedWMOReference &
Noggit::WMO::ScopedWMOReference::operator=(ScopedWMOReference &&other) {
  if (this != &other) {
    std::swap(_valid, other._valid);
    std::swap(_file_key, other._file_key);
    std::swap(_wmo, other._wmo);
    std::swap(_context, other._context);
    other._valid = false;
  }
  return *this;
}

Noggit::WMO::ScopedWMOReference::~ScopedWMOReference() {
  if (_valid) {
    WMOManager::_.erase(_file_key, _context);
  }
}

Noggit::WMO::WMO *Noggit::WMO::ScopedWMOReference::operator->() const {
  return _wmo;
}

Noggit::WMO::WMO *Noggit::WMO::ScopedWMOReference::get() const { return _wmo; }
