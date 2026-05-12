// This file is part of Noggit3, licensed under GNU General Public License
// (version 3).

#pragma once

#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <noggit/MapEnums.hpp>
#include <queue>
#include <thread>

class SceneObject;
class World;

namespace Noggit {
struct instance_update;

class WorldTileUpdateQueue {
public:
  WorldTileUpdateQueue(World *world);
  ~WorldTileUpdateQueue();

  WorldTileUpdateQueue() = delete;
  WorldTileUpdateQueue(WorldTileUpdateQueue const &) = delete;
  WorldTileUpdateQueue(WorldTileUpdateQueue &&) = delete;
  WorldTileUpdateQueue &operator=(WorldTileUpdateQueue const &) = delete;
  WorldTileUpdateQueue &operator=(WorldTileUpdateQueue &&) = delete;

  void waitForAllUpdate();

  void queueUpdate(SceneObject *instance, ModelUpdate type);

private:
  void processQueue();

private:
  World *_world;

  std::atomic<bool> _stop = {false};
  std::mutex _mutex;
  std::condition_variable _state_changed;

  // only use one thread
  std::unique_ptr<std::thread> _thread;

  std::queue<std::unique_ptr<instance_update>> _update_queue;
};
} // namespace Noggit
