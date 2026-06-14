#include "raylibRmlUi.h"
#include "raylibFileInterface.h"
#include "raylibRenderInterface.h"
#include "raylibSystemInterface.h"
#include <raylib.h>


CustomSystemInterface *g_systemInterface = nullptr;
CustomFileInterface *g_fileInterface = nullptr;
CustomRenderInterface *g_renderInterface = nullptr;

Rml::Context *RaylibRmlUi::context = nullptr;

bool RaylibRmlUi::Initialize(int windowWidth, int windowHeight) {
  g_systemInterface = new CustomSystemInterface();
  g_fileInterface = new CustomFileInterface();
  g_renderInterface = new CustomRenderInterface();

  Rml::SetSystemInterface(g_systemInterface);
  Rml::SetFileInterface(g_fileInterface);
  Rml::SetRenderInterface(g_renderInterface);

  if (!Rml::Initialise()) {
    return false;
  }

  context =
      Rml::CreateContext("main", Rml::Vector2i(windowWidth, windowHeight));
  if (!context) {
    Rml::Shutdown();
    return false;
  }

  Rml::Debugger::Initialise(context);

  return true;
}

void RaylibRmlUi::DeInitialize() {
  if (context) {
    // Documents attached to context are automatically destroyed.
    context = nullptr;
  }

  Rml::Shutdown();

  delete g_systemInterface;
  delete g_fileInterface;
  delete g_renderInterface;
}

void RaylibRmlUi::Update() {
  if (!context)
    return;

  // Update context bounds in case window resized
  context->SetDimensions(Rml::Vector2i(GetScreenWidth(), GetScreenHeight()));

  g_systemInterface->UpdateInput(context);
  context->Update();
}

void RaylibRmlUi::Draw() {
  if (!context)
    return;

  // Optional: Only render if we have visibility or something actually changed.
  // For Raylib, drawing every frame is standard.

  // Note: We use alpha blending for text and UI shapes!
  BeginBlendMode(BLEND_ALPHA);

  // Render
  context->Render();

  EndBlendMode();
}

void RaylibRmlUi::LoadFont(const char *path) {
  if (!Rml::LoadFontFace(path)) {
    Rml::Log::Message(Rml::Log::LT_ERROR, "Failed to load font: %s", path);
  }
}

void RaylibRmlUi::EnableDebugger() { Rml::Debugger::SetVisible(true); }

Rml::ElementDocument *RaylibRmlUi::LoadDocument(const char *path) {
  if (!context)
    return nullptr;
  auto doc = context->LoadDocument(path);
  if (!doc) {
    Rml::Log::Message(Rml::Log::LT_ERROR, "Failed to load document: %s", path);
  } else {
    doc->Show();
  }
  return doc;
}