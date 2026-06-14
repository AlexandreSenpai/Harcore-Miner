#pragma once
#include "engine/core/camera.h"
#include "engine/ecs/entity.h"
#include <functional>
#include <memory>
#include <string>
#include <vector>

class LevelGenerator;

class IScene {
protected:
  std::vector<IEntity *> entities;
  const char *name;
  GameCamera camera;

  std::vector<IEntity *> pendingAdditions;
  std::vector<std::string> pendingRemovals;
  std::vector<std::function<void()>> pendingTypeRemovals;

  void Debug();

public:
  IScene(const char *name);
  virtual ~IScene() = 0;

  static std::unique_ptr<IScene *> Factory();

  const char *GetName() const { return this->name; }
  void AddEntity(IEntity *entity);
  void DestroyEntity(std::string id);

  template <typename T> void DestroyEntitiesByType() {
    for (auto it = entities.begin(); it != entities.end();) {
      if (dynamic_cast<T *>(*it) != nullptr) {
        delete *it;
        it = entities.erase(it);
      } else {
        ++it;
      }
    }
  }

  // Deferred (safe) versions — queued and applied via FlushPendingChanges()
  void RequestAddEntity(IEntity *entity);
  void RequestDestroyEntity(std::string id);

  template <typename T> void RequestDestroyEntitiesByType() {
    pendingTypeRemovals.push_back([this]() { this->DestroyEntitiesByType<T>(); });
  }

  void FlushPendingChanges();

  IEntity *GetEntityById(const std::string &id);
  const std::vector<IEntity *> &GetEntities() const;

  GameCamera &GetCamera() { return camera; }
  const GameCamera &GetCamera() const { return camera; }

  virtual LevelGenerator *GetLevelGenerator() const { return nullptr; }

  virtual void Update() = 0;
  virtual void Draw();
};