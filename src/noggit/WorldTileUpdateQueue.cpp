// This file is part of Noggit3, licensed under GNU General Public License
// (version 3).

#include <noggit/WorldTileUpdateQueue.hpp>

#include <noggit/Log.h>
#include <noggit/ModelInstance.h>
#include <noggit/WMOInstance.h>
#include <noggit/World.h>

namespace Noggit {
struct instance_update {
  SceneObject *instance;
  ModelUpdate update_type;

  instance_update() = delete;
  instance_update(instance_update const &) = delete;
  instance_update(instance_update &&) = default;
  instance_update &operator=(instance_update const &) = delete;
  instance_update &operator=(instance_update &&) = default;

  instance_update(SceneObject *obj, ModelUpdate type)
      : instance(obj), update_type(type) {}

  void apply(World *const world) {
    instance->instance_model()->wait_until_loaded();
    auto &extents(instance->getExtents());
    TileIndex start(extents[0]), end(extents[1]);

    for (size_t z = start.z; z <= end.z; ++z) {
      for (size_t x = start.x; x <= end.x; ++x) {
        world->mapIndex.updateModelTile(TileIndex(x, z), update_type, instance);
      }
    }
  }
};

WorldTileUpdateQueue::WorldTileUpdateQueue(World *world) : _world(world) {
  _thread =
      std::make_unique<std::thread>(&WorldTileUpdateQueue::processQueue, this);
}

WorldTileUpdateQueue::~WorldTileUpdateQueue() {
  _stop = true;
  _state_changed.notify_all();

  _thread->join();

  if (!_update_queue.empty()) {
    LogError << "Update queue deleted with some update pending !" << std::endl;
  }
}

void WorldTileUpdateQueue::waitForAllUpdate() {
  std::unique_lock<std::mutex> lock(_mutex);

  _state_changed.wait(lock, [&] { return _update_queue.empty(); });
}

void WorldTileUpdateQueue::queueUpdate(SceneObject *instance,
                                        ModelUpdate type) {
  {
    std::lock_guard<std::mutex> const lock(_mutex);

    _update_queue.emplace(new instance_update(instance, type));
    _state_changed.notify_one();
  }
  // make sure deletion are done here
  // otherwise the instance get deleted
  if (type == ModelUpdate::remove) {
    // wait for all update to make sure they are done in the right order
    waitForAllUpdate();
  }
}

void WorldTileUpdateQueue::processQueue() {
  instance_update *update;

  while (!_stop.load()) {
    {
      std::unique_lock<std::mutex> lock(_mutex);

      _state_changed.wait(
          lock, [&] { return _stop.load() || !_update_queue.empty(); });

      if (_stop.load()) {
        return;
      }

      update = _update_queue.front().get();
    }

    update->apply(_world);

    {
      std::lock_guard<std::mutex> const lock(_mutex);
      _update_queue.pop();
      _state_changed.notify_all();
    }
  }
}
} // namespace Noggit
