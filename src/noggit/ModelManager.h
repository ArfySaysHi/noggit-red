// This file is part of Noggit3, licensed under GNU General Public License
// (version 3).

#pragma once

#include <ClientData.hpp>
#include <noggit/AsyncObjectMultimap.hpp>
#include <noggit/ContextObject.hpp>
#include <noggit/Model.h>

class Model;

class ModelManager {
public:
  static void resetAnim();
  static void updateEmitters(float dt);
  static void clear_hidden_models();
  static void unload_all(Noggit::NoggitRenderContext context);

  static void report();

private:
  friend struct ScopedModelReference;
  static Noggit::AsyncObjectMultimap<Model> _;
};

struct ScopedModelReference {
  ScopedModelReference(BlizzardArchive::Listfile::FileKey const &file_key,
                         Noggit::NoggitRenderContext context)

      : _valid(true), _file_key(file_key),
        _model(ModelManager::_.emplace(_file_key, context)), _context(context)

  {}

  ScopedModelReference(ScopedModelReference const &other)
      : _valid(other._valid), _file_key(other._file_key),
        _model(ModelManager::_.emplace(_file_key, other._context)),
        _context(other._context) {}
  ScopedModelReference &operator=(ScopedModelReference const &other) {
    _valid = other._valid;
    _file_key = other._file_key;
    _model = ModelManager::_.emplace(_file_key, other._context);
    _context = other._context;
    return *this;
  }

  ScopedModelReference(ScopedModelReference &&other)
      : _valid(other._valid), _file_key(other._file_key), _model(other._model),
        _context(other._context) {
    other._valid = false;
  }
  ScopedModelReference &operator=(ScopedModelReference &&other) {
    std::swap(_valid, other._valid);
    std::swap(_file_key, other._file_key);
    std::swap(_model, other._model);
    std::swap(_context, other._context);
    other._valid = false;
    return *this;
  }

  ~ScopedModelReference() {
    if (_valid) {
      ModelManager::_.erase(_file_key, _context);
    }
  }

  Model *operator->() const { return _model; }

  [[nodiscard]]
  Model *get() const {
    return _model;
  }

private:
  bool _valid;
  BlizzardArchive::Listfile::FileKey _file_key;
  Model *_model;
  Noggit::NoggitRenderContext _context;
};
