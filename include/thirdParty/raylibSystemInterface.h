#pragma once

#include <RmlUi/Core/Input.h>
#include <RmlUi/Core/SystemInterface.h>


class CustomSystemInterface : public Rml::SystemInterface {
public:
  CustomSystemInterface() = default;
  virtual ~CustomSystemInterface() = default;

  // Returns the time in seconds since the application started.
  double GetElapsedTime() override;

  // Logs the specified message.
  bool LogMessage(Rml::Log::Type type, const Rml::String &message) override;

  // Helper functions to pass Raylib inputs into RmlUi.
  void UpdateInput(Rml::Context *context);
};