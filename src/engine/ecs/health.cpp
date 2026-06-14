#include "engine/ecs/health.h"
#include <algorithm>
#include <imgui.h>

HealthComponent::HealthComponent(IEntity *entity, float maxHealth)
    : IComponent(entity, "HealthComponent"), maxHealth(maxHealth),
      currentHealth(maxHealth), isDead(false) {}

void HealthComponent::TakeDamage(float amount) {
  if (isDead)
    return;

  currentHealth -= amount;
  if (currentHealth <= 0.0f) {
    currentHealth = 0.0f;
    isDead = true;
  }
}

void HealthComponent::Heal(float amount) {
  if (isDead)
    return;

  currentHealth = std::min(currentHealth + amount, maxHealth);
}

float HealthComponent::GetLifePercentage() const {
  if (maxHealth <= 0.0f)
    return 0.0f;
  return currentHealth / maxHealth;
}

void HealthComponent::Debug() {
  IComponent::Debug();
  ImGui::ProgressBar(GetLifePercentage(), ImVec2(0.0f, 0.0f));
  ImGui::Text("HP: %.1f / %.1f", currentHealth, maxHealth);
  ImGui::Text("Status: %s", isDead ? "Dead" : "Alive");
}
