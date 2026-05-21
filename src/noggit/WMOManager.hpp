#pragma once

#include <Listfile.hpp>
#include <noggit/AsyncObjectMultimap.hpp>
#include <noggit/ContextObject.hpp>

class WMO;

class WMOManager {
public:
  static void report();
  static void clear_hidden_wmos();
  static void unload_all(Noggit::NoggitRenderContext context);

private:
  friend struct ScopedWMOReference;
  static Noggit::AsyncObjectMultimap<WMO> _;
};

struct ScopedWMOReference {
  ScopedWMOReference(BlizzardArchive::Listfile::FileKey const &file_key,
                     Noggit::NoggitRenderContext context);

  ScopedWMOReference(ScopedWMOReference const &other);
  ScopedWMOReference &operator=(ScopedWMOReference const &other);

  ScopedWMOReference(ScopedWMOReference &&other);
  ScopedWMOReference &operator=(ScopedWMOReference &&other);

  ~ScopedWMOReference();

  WMO *operator->() const;
  WMO *get() const;

private:
  bool _valid;
  BlizzardArchive::Listfile::FileKey _file_key;
  WMO *_wmo;
  Noggit::NoggitRenderContext _context;
};
