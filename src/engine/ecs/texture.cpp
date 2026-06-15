#include "engine/ecs/texture.h"
#include "engine/core/asset_manager.h"
#include "engine/ecs/entity.h"
#include <cmath>
#include <imgui.h>
#include <iostream>
#include <raylib.h>

TextureComponent::TextureComponent(IEntity *entity, const char *texturePath,
                                   bool playAnimation)
    : IComponent(entity, "TextureComponent") {

  if (texturePath != nullptr) {
    this->texture = AssetManager::GetInstance()->LoadTexture(texturePath);
    this->currentTexturePath = std::string(texturePath);
  }

  this->playAnimation = playAnimation;
}

TextureComponent::~TextureComponent() {
  this->RemoveShader();
}

void TextureComponent::Draw() {
  if (!this->IsEnabled()) {
    return;
  }

  Vector2 *pos = this->entity->GetPosition();
  Vector2 *size = this->entity->GetTransform();

  Rectangle sourceRec = {0.0f, 0.0f, size->x, size->y};
  Color frameTint = WHITE;

  if (!this->currentAnimation.empty()) {
    Animation &animation = this->animations[this->currentAnimation];

    sourceRec.x = animation.currentFrame * animation.frameSize;
    sourceRec.y = animation.frameRow * animation.frameSize;
    sourceRec.width = animation.frameSize;
    sourceRec.height = animation.frameSize;

    frameTint = animation.frameTint;
    this->texture = animation.texture;
  }

  if (this->flipX) {
    sourceRec.width = -sourceRec.width;
  }

  if (this->flipY) {
    sourceRec.height = -sourceRec.height;
  }

  Vector2 position = {std::floor(pos->x), std::floor(pos->y)};

  if (!this->currentShaderPath.empty()) {
    ::BeginShaderMode(this->shader);
    ::DrawTextureRec(this->texture, sourceRec, position, frameTint);
    ::EndShaderMode();
  } else {
    ::DrawTextureRec(this->texture, sourceRec, position, frameTint);
  }
}

void TextureComponent::Debug() {
  IComponent::Debug();

  ImGui::Checkbox("Flip X", &this->flipX);
  ImGui::Checkbox("Flip Y", &this->flipY);

  ImGui::Checkbox("Should Play Animation", &this->playAnimation);
  ImGui::Combo("Animation Playback Type", (int *)&this->animationPlaybackType,
               "Loop\0Once\0Step\0", 3);

  ImGui::SeparatorText("Animations");
  for (auto &animationInfo : this->animations) {
    ImGui::PushID(animationInfo.first.c_str());
    ImGui::Text("Animation Name: %s", animationInfo.first.c_str());
    ImGui::InputInt("Frames Row", &animationInfo.second.frameRow);
    ImGui::InputInt("Total Frames", &animationInfo.second.totalFrames);
    ImGui::InputInt("Frame Size", &animationInfo.second.frameSize);
    ImGui::InputInt("Frames Per Second", &animationInfo.second.framesPerSecond);
    ImGui::PopID();
  }
}

void TextureComponent::Update() {
  if (!this->IsEnabled() || !this->playAnimation) {
    return;
  }

  if (this->currentAnimation.empty()) {
    return;
  }

  Animation &animation = this->animations[this->currentAnimation];

  if (this->animationPlaybackType == AnimationPlaybackType::STEP) {
    return;
  }

  animation.frameTimer += GetFrameTime();
  if (animation.frameTimer >= 1.0f / animation.framesPerSecond) {
    animation.currentFrame++;

    if (this->animationResult != nullptr) {
      this->animationResult->currentFrame = animation.currentFrame;
      this->animationResult->state = AnimationPlaybackState::PLAYING;
    }

    animation.frameTimer = 0.0f;

    if (animation.currentFrame >= animation.totalFrames) {
      animation.currentFrame = 0;
      if (this->animationPlaybackType == AnimationPlaybackType::ONCE) {
        this->playAnimation = false;

        if (this->animationResult != nullptr) {
          this->animationResult->state = AnimationPlaybackState::FINISHED;
        }
      }
    }
  }
}

void TextureComponent::PlayAnimation(bool play, AnimationResult *result,
                                     AnimationPlaybackType type) {
  this->playAnimation = play;
  this->animationPlaybackType = type;

  if (result != nullptr) {
    result->state = AnimationPlaybackState::PLAYING;
    result->currentFrame =
        this->animations[this->currentAnimation].currentFrame;
    this->animationResult = result;
  }
}

void TextureComponent::FlipHorizontally(bool flipX) { this->flipX = flipX; }

void TextureComponent::FlipVertically(bool flipY) { this->flipY = flipY; }

void TextureComponent::AddAnimation(std::string name, Animation animation) {
  animation.texture =
      AssetManager::GetInstance()->LoadTexture(animation.texturePath.c_str());
  this->animations[name] = animation;
}

void TextureComponent::SetCurrentAnimation(std::string name) {
  if (this->animations.find(name) == this->animations.end()) {
    std::cerr << "Animation not found: " << name << std::endl;
    return;
  }
  this->currentAnimation = name;
}

void TextureComponent::SetAnimationFrame(int frame) {
  if (this->currentAnimation.empty())
    return;

  Animation &animation = this->animations[this->currentAnimation];
  if (frame >= 0 && frame < animation.totalFrames) {
    animation.currentFrame = frame;
  }
}

void TextureComponent::SetShader(const char *shaderPath) {
  this->shader = AssetManager::GetInstance()->LoadShader(shaderPath);
  this->currentShaderPath = std::string(shaderPath);
}

void TextureComponent::RemoveShader() {
  if (!this->currentShaderPath.empty()) {
    AssetManager::GetInstance()->UnloadShader(this->currentShaderPath.c_str());
    this->currentShaderPath.clear();
  }
}