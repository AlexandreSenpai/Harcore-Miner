#include "raylibSystemInterface.h"
#include <RmlUi/Core/Context.h>
#include <raylib.h>


double CustomSystemInterface::GetElapsedTime() { return GetTime(); }

bool CustomSystemInterface::LogMessage(Rml::Log::Type type,
                                       const Rml::String &message) {
  int raylibLogLevel = LOG_INFO;
  switch (type) {
  case Rml::Log::LT_ALWAYS:
  case Rml::Log::LT_INFO:
    raylibLogLevel = LOG_INFO;
    break;
  case Rml::Log::LT_ASSERT:
  case Rml::Log::LT_ERROR:
    raylibLogLevel = LOG_ERROR;
    break;
  case Rml::Log::LT_WARNING:
    raylibLogLevel = LOG_WARNING;
    break;
  case Rml::Log::LT_DEBUG:
    raylibLogLevel = LOG_DEBUG;
    break;
  default:
    raylibLogLevel = LOG_INFO;
    break;
  }

  TraceLog(raylibLogLevel, "[RmlUi] %s", message.c_str());
  return true; // Return true to continue execution
}

// Map Raylib keys to RmlUi KeyIdentifiers
static Rml::Input::KeyIdentifier ConvertKey(int raylibKey) {
  switch (raylibKey) {
  case KEY_ESCAPE:
    return Rml::Input::KI_ESCAPE;
  case KEY_SPACE:
    return Rml::Input::KI_SPACE;
  case KEY_ENTER:
    return Rml::Input::KI_RETURN;
  case KEY_TAB:
    return Rml::Input::KI_TAB;
  case KEY_BACKSPACE:
    return Rml::Input::KI_BACK;
  case KEY_UP:
    return Rml::Input::KI_UP;
  case KEY_DOWN:
    return Rml::Input::KI_DOWN;
  case KEY_LEFT:
    return Rml::Input::KI_LEFT;
  case KEY_RIGHT:
    return Rml::Input::KI_RIGHT;
  case KEY_ZERO:
    return Rml::Input::KI_0;
  case KEY_ONE:
    return Rml::Input::KI_1;
  case KEY_TWO:
    return Rml::Input::KI_2;
  case KEY_THREE:
    return Rml::Input::KI_3;
  case KEY_FOUR:
    return Rml::Input::KI_4;
  case KEY_FIVE:
    return Rml::Input::KI_5;
  case KEY_SIX:
    return Rml::Input::KI_6;
  case KEY_SEVEN:
    return Rml::Input::KI_7;
  case KEY_EIGHT:
    return Rml::Input::KI_8;
  case KEY_NINE:
    return Rml::Input::KI_9;
  case KEY_A:
    return Rml::Input::KI_A;
  case KEY_B:
    return Rml::Input::KI_B;
  case KEY_C:
    return Rml::Input::KI_C;
  case KEY_D:
    return Rml::Input::KI_D;
  case KEY_E:
    return Rml::Input::KI_E;
  case KEY_F:
    return Rml::Input::KI_F;
  case KEY_G:
    return Rml::Input::KI_G;
  case KEY_H:
    return Rml::Input::KI_H;
  case KEY_I:
    return Rml::Input::KI_I;
  case KEY_J:
    return Rml::Input::KI_J;
  case KEY_K:
    return Rml::Input::KI_K;
  case KEY_L:
    return Rml::Input::KI_L;
  case KEY_M:
    return Rml::Input::KI_M;
  case KEY_N:
    return Rml::Input::KI_N;
  case KEY_O:
    return Rml::Input::KI_O;
  case KEY_P:
    return Rml::Input::KI_P;
  case KEY_Q:
    return Rml::Input::KI_Q;
  case KEY_R:
    return Rml::Input::KI_R;
  case KEY_S:
    return Rml::Input::KI_S;
  case KEY_T:
    return Rml::Input::KI_T;
  case KEY_U:
    return Rml::Input::KI_U;
  case KEY_V:
    return Rml::Input::KI_V;
  case KEY_W:
    return Rml::Input::KI_W;
  case KEY_X:
    return Rml::Input::KI_X;
  case KEY_Y:
    return Rml::Input::KI_Y;
  case KEY_Z:
    return Rml::Input::KI_Z;
  default:
    return Rml::Input::KI_UNKNOWN;
  }
}

// Gather currently held modifier keys
static int GetKeyModifierState() {
  int state = 0;
  if (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL))
    state |= Rml::Input::KM_CTRL;
  if (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT))
    state |= Rml::Input::KM_SHIFT;
  if (IsKeyDown(KEY_LEFT_ALT) || IsKeyDown(KEY_RIGHT_ALT))
    state |= Rml::Input::KM_ALT;
  return state;
}

void CustomSystemInterface::UpdateInput(Rml::Context *context) {
  if (!context)
    return;

  int modifiers = GetKeyModifierState();

  // Mouse Movement
  Vector2 mouseDelta = GetMouseDelta();
  if (mouseDelta.x != 0.0f || mouseDelta.y != 0.0f) {
    Vector2 mousePos = GetMousePosition();
    context->ProcessMouseMove((int)mousePos.x, (int)mousePos.y, modifiers);
  }

  // Mouse Buttons
  if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    context->ProcessMouseButtonDown(0, modifiers);
  if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
    context->ProcessMouseButtonUp(0, modifiers);

  if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
    context->ProcessMouseButtonDown(1, modifiers);
  if (IsMouseButtonReleased(MOUSE_BUTTON_RIGHT))
    context->ProcessMouseButtonUp(1, modifiers);

  if (IsMouseButtonPressed(MOUSE_BUTTON_MIDDLE))
    context->ProcessMouseButtonDown(2, modifiers);
  if (IsMouseButtonReleased(MOUSE_BUTTON_MIDDLE))
    context->ProcessMouseButtonUp(2, modifiers);

  // Mouse Wheel
  float wheelMove = GetMouseWheelMove();
  if (wheelMove != 0.0f) {
    // RmlUi expects positive up, negative down for wheel (y-axis inverted
    // depending on backend, testing shows -wheelMove matches expectations
    // usually)
    context->ProcessMouseWheel(-wheelMove, modifiers);
  }

  // Keyboard Key Presses
  int key = GetKeyPressed();
  while (key > 0) {
    context->ProcessKeyDown(ConvertKey(key), modifiers);
    key = GetKeyPressed(); // Check for next key
  }

  // Note: Raylib doesn't have a GetKeyReleased queue, so we manually check the
  // ones we care about or handle them in a different way. For a standard
  // integration, checking state of known keys each frame is reliable but we
  // might miss single-frame releases. This simple polling approach works for
  // basic UI.

  // Process text input (unicode)
  int charPressed = GetCharPressed();
  while (charPressed > 0) {
    context->ProcessTextInput(charPressed);
    charPressed = GetCharPressed();
  }
}