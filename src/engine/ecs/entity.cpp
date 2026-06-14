#include "engine/ecs/entity.h"
#include "engine/ecs/component.h"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <imgui.h>
#include <raylib.h>

IEntity::IEntity(const char *name) {
  this->name = name;

  boost::uuids::random_generator gen;
  boost::uuids::uuid u = gen();

  this->id = boost::uuids::to_string(u);
}

IEntity::~IEntity() {
  for (IComponent *component : this->components) {
    delete component;
  }
}

void IEntity::Debug() {
  std::string currentId = this->GetId();
  ImGui::PushID(currentId.c_str());

  ImGui::Text("Entity ID: %s", currentId.c_str());
  ImGui::Text("Entity Name: %s", this->GetName().c_str());

  // Entity Transform
  Vector2 *size = this->GetTransform();
  ImGui::InputFloat2("Transform", (float *)size);
  this->SetTransform(*size);

  // Entity Position
  Vector2 *pos = this->GetPosition();
  ImGui::InputFloat2("Position", (float *)pos);
  this->SetPosition(*pos);

  // Entity Components
  ImGui::SeparatorText("Components");
  for (IComponent *component : this->components) {
    if (ImGui::CollapsingHeader(component->GetName())) {
      ImGui::PushID(component->GetName());
      ImGui::Indent();

      bool isEnabled = component->IsEnabled();
      ImGui::Checkbox("Enabled", &isEnabled);
      component->SetEnabled(isEnabled);
      component->Debug();

      ImGui::Unindent();
      ImGui::PopID();
    }
  }

  ImGui::PopID();
}

std::string IEntity::GetId() { return this->id; }

std::string IEntity::GetName() { return this->name; }

void IEntity::SetName(std::string name) { this->name = name; }

Vector2 *IEntity::GetTransform() { return &this->transform; }

void IEntity::SetTransform(Vector2 newTransform) {
  this->transform = newTransform;
}

Vector2 *IEntity::GetPosition() { return &this->position; }

void IEntity::SetPosition(Vector2 newPosition) { this->position = newPosition; }

void IEntity::AddComponent(IComponent *component) {
  this->components.push_back(component);
}

void IEntity::Update() {
  for (IComponent *component : this->components) {
    component->Update();
  }
}

void IEntity::Draw() {
  for (IComponent *component : this->components) {
    component->Draw();
  }
}