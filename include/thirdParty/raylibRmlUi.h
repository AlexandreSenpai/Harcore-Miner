#pragma once

#include <RmlUi/Core.h>
#include <RmlUi/Debugger.h>

class RaylibRmlUi {
public:
  static bool Initialize(int windowWidth, int windowHeight);
  static void DeInitialize();

  static void Update();
  static void Draw();

  static void LoadFont(const char *path);
  static void EnableDebugger();

  static Rml::ElementDocument *LoadDocument(const char *path);

private:
  static Rml::Context *context;
};