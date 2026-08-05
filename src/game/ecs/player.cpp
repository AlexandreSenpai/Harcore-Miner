#include "game/ecs/player.h"
#include "engine/ecs/circle_collider.h"
#include "engine/ecs/collider.h"
#include "engine/ecs/move_2d.h"
#include "engine/ecs/rigidbody_2d.h"
#include "engine/ecs/texture.h"

#include "engine/ecs/attack.h"
#include "game/shop/shop.h"
#include <memory>

Player::Player() : IEntity("Player") {
  this->SetTransform({16, 16});
  this->AddComponent(new Move2DComponent(this, 100.0f));
  this->AddComponent(new ColliderComponent(this, 11, 13, 1.6f, 3.0f));
  this->AddComponent(new CircleColliderComponent(this, 30.0f));
  this->AddComponent(new RigidBody2DComponent(this));
  this->AddComponent(new AttackComponent(this, 1.0f));

  TextureComponent *texture = new TextureComponent(this);
  texture->AddAnimation(
      "idle", {
                  .texturePath = "src/game/assets/Player/player_anim.png",
                  .frameRow = 0,
                  .totalFrames = 2,
                  .frameWidth = 16,
                  .frameHeight = 16,
                  .framesPerSecond = 4,
              });
  texture->AddAnimation(
      "running", {
                     .texturePath = "src/game/assets/Player/player_anim.png",
                     .frameRow = 1,
                     .totalFrames = 2,
                     .frameWidth = 16,
                     .frameHeight = 16,
                     .framesPerSecond = 4,
                 });

  texture->SetCurrentAnimation("idle");
  texture->PlayAnimation(true);

  this->AddComponent(texture);

  EventSystem::GetInstance()->AddListener(
      EventType::ON_SKILL_PURCHASED, [this](void *data) {
        this->OnSkillPurchaseEvent(static_cast<SkillPurchasedEvent *>(data));
      });
};

Player::~Player() {};

void Player::AddSkill(std::unique_ptr<Skill> skill) {
  if (!skill)
    return;

  std::string skillName = skill->name;
  auto it = this->skills.find(skillName);

  if (it != this->skills.end()) {
    it->second->LevelUp();
  } else {
    this->skills[skillName] = std::move(skill);
  }
}

void Player::RemoveSkill(const std::string &name) { this->skills.erase(name); }

Skill *Player::GetSkill(const std::string &name) {
  auto it = this->skills.find(name);
  if (it != this->skills.end()) {
    return it->second.get();
  }
  return nullptr;
}

void Player::OnSkillPurchaseEvent(SkillPurchasedEvent *event) {
  if (event && event->skill) {
    event->skill->SetPlayerReference(this);
    this->AddSkill(std::move(event->skill));
  }
}

void Player::Update() {

  for (auto &[name, skill] : this->skills) {
    skill->Update();
  }

  IEntity::Update();
};

void Player::Draw() {
  for (auto &[name, skill] : this->skills) {
    skill->Draw();
  }

  IEntity::Draw();
};