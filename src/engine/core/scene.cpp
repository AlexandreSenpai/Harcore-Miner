#include "engine/core/scene.h"
#include <imgui.h>
#include <string>

IScene::IScene(const char *name) { this->name = name; }

IScene::~IScene() {
  for (int i = 0; i < this->entities.size(); i++) {
    delete this->entities[i];
  }
  entities.clear();
}

void IScene::AddEntity(IEntity *entity) { this->entities.push_back(entity); }

void IScene::DestroyEntity(std::string id) {
  for (int i = 0; i < this->entities.size(); i++) {
    if (this->entities[i]->GetId() == id) {
      delete this->entities[i];
      this->entities.erase(this->entities.begin() + i);
      return;
    }
  }
}

void IScene::RequestAddEntity(IEntity *entity) {
  this->pendingAdditions.push_back(entity);
}

void IScene::RequestDestroyEntity(std::string id) {
  this->pendingRemovals.push_back(id);
}

void IScene::FlushPendingChanges() {
  // Process type-based removals first
  for (auto &removalFn : this->pendingTypeRemovals) {
    removalFn();
  }
  this->pendingTypeRemovals.clear();

  // Process individual removals
  for (const std::string &id : this->pendingRemovals) {
    this->DestroyEntity(id);
  }
  this->pendingRemovals.clear();

  // Process additions
  for (IEntity *entity : this->pendingAdditions) {
    this->AddEntity(entity);
  }
  this->pendingAdditions.clear();
}

IEntity *IScene::GetEntityById(const std::string &id) {
  for (IEntity *entity : this->entities) {
    if (entity->GetId() == id) {
      return entity;
    }
  }
  return nullptr;
}

const std::vector<IEntity *> &IScene::GetEntities() const {
  return this->entities;
}

void IScene::Debug() {
  ImGui::Begin("Scene Inspector");
  ImGui::Text("Scene: %s", this->name);
  ImGui::Text("FPS: %d", ::GetFPS());
  ImGui::Separator();

  if (ImGui::CollapsingHeader("Entities", ImGuiTreeNodeFlags_DefaultOpen)) {
    ImGui::Indent();
    for (int i = 0; i < this->entities.size(); i++) {
      IEntity *currentEntity = this->entities[i];
      std::string currentId = currentEntity->GetId();

      ImGui::PushID(currentId.c_str());

      if (ImGui::CollapsingHeader(currentEntity->GetName().c_str())) {
        ImGui::Indent();
        currentEntity->Debug();
        ImGui::Unindent();
      }

      ImGui::PopID();
    }
    ImGui::Unindent();
  }
  ImGui::End();
}

void IScene::Draw() { this->Debug(); }