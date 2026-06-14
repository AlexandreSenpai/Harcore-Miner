#pragma once

#include "component.h"
#include "component_metadata.h"
#include "engine/ecs/entity.h"
#include <map>
#include <raylib.h>

enum class AnimationPlaybackType { ONCE, LOOP, STEP };
enum class AnimationPlaybackState { PLAYING, FINISHED };

struct AnimationResult {
  AnimationPlaybackState state;
  int currentFrame;
};

struct Animation {
  std::string texturePath;
  Texture2D texture;
  int frameRow = 0;
  int totalFrames;
  int frameSize;
  int framesPerSecond = 1;
  float frameTimer = 0.0f;
  int currentFrame = 0;
  Color frameTint = WHITE;
};

class TextureComponent : public IComponent {
protected:
  Texture2D texture;
  std::string currentTexturePath;

  bool playAnimation = false;
  AnimationPlaybackType animationPlaybackType = AnimationPlaybackType::LOOP;
  AnimationResult *animationResult = nullptr;

  std::map<std::string, Animation> animations;
  std::string currentAnimation = "";

  bool flipX = false;
  bool flipY = false;

  Shader shader;
  std::string currentShaderPath;

public:
  static const int ID = ComponentType::TEXTURE;

  TextureComponent(IEntity *entity, const char *texturePath = nullptr,
                   bool playAnimation = false);
  ~TextureComponent() override;

  void Draw() override;

  void Update() override;
  void Debug() override;

  int GetId() const override { return this->ID; }

  void AddAnimation(std::string name, Animation animation);
  void SetCurrentAnimation(std::string name);
  void PlayAnimation(bool play, AnimationResult *result = nullptr,
                     AnimationPlaybackType type = AnimationPlaybackType::LOOP);
  void SetAnimationFrame(int frame);
  void FlipHorizontally(bool flipX = false);
  void FlipVertically(bool flipY = false);

  void SetShader(const char *shaderPath);
  void RemoveShader();
};