#include "game/level/timer.h"
#include "engine/ecs/entity.h"
#include <imgui.h>
#include <raylib.h>
#include <raymath.h>

Timer::Timer(float duration, TimeDirection direction, int precision,
             int fontSize)
    : IEntity("Timer") {
  this->timer = 0.0f;

  this->maxDuration = duration;
  this->isFinished = false;
  this->started = false;
  this->direction = direction;
  this->precision = precision;
  this->fontSize = fontSize;

  this->SetDirection(direction);
  this->SetPosition({355.0f, 250.0f});
}

Timer::~Timer() {}

void Timer::Draw() {
  Vector2 origin = {0, 0};
  float rotation = 0.0f;
  float spacing = 1.0f;
  Color tint = WHITE;

  const char *timeString = ::TextFormat("%0.0f", this->currentTime);

  // text align center
  origin.x = ::MeasureText(timeString, this->fontSize) / 2.0f;
  origin.y = this->fontSize / 2.0f;

  ::DrawTextPro(::GetFontDefault(), timeString, *this->GetPosition(), origin,
                rotation, this->fontSize, spacing, tint);
}

void Timer::Update() {
  if (this->isFinished || !this->started) {
    return;
  }

  this->timer += GetFrameTime();

  if (this->timer >= 0.1f) {
    if (this->direction == TimeDirection::COUNTUP) {
      this->currentTime =
          Clamp(this->currentTime + 0.1f, 0.0f, this->maxDuration);
    } else {
      this->currentTime =
          Clamp(this->currentTime - 0.1f, 0.0f, this->maxDuration);
    }

    this->timer = 0.0f;

    if (this->direction == TimeDirection::COUNTUP &&
        this->currentTime >= this->maxDuration) {
      this->isFinished = true;
      this->started = false;
      this->DispatchTimerFinish();
    } else if (this->direction == TimeDirection::COUNTDOWN &&
               this->currentTime <= 0.0f) {
      this->isFinished = true;
      this->started = false;
      this->DispatchTimerFinish();
    }
  }
}

void Timer::Debug() {
  IEntity::Debug();

  ImGui::Text("Timer: %f", this->timer);
  ImGui::InputFloat("Current Time", &this->currentTime);
  ImGui::InputFloat("Max Duration", &this->maxDuration);
  ImGui::Checkbox("Is Finished", &this->isFinished);
  ImGui::Checkbox("Started", &this->started);

  ImGui::InputInt("Font Size", &this->fontSize);
  ImGui::Combo("Direction", (int *)&this->direction, "COUNTDOWN\0COUNTUP\0", 2);

  ImGui::SeparatorText("Controls");

  if (ImGui::Button("Start")) {
    this->Start();
  }
  ImGui::SameLine();
  if (ImGui::Button("Pause")) {
    this->Pause();
  }
  ImGui::SameLine();
  if (ImGui::Button("Resume")) {
    this->Resume();
  }
  ImGui::SameLine();
  if (ImGui::Button("Stop")) {
    this->Stop();
  }

  ImGui::SeparatorText("Time Controls");

  if (ImGui::Button("Add Time")) {
    this->AddTime(10.0f);
  }
  ImGui::SameLine();
  if (ImGui::Button("Remove Time")) {
    this->RemoveTime(10.0f);
  }
}

void Timer::AddTime(float time) {
  this->currentTime += time;
  if (this->currentTime > this->maxDuration) {
    this->maxDuration = this->currentTime;
  }
}

void Timer::RemoveTime(float time) {
  this->currentTime -= time;
  if (this->currentTime < 0.0f) {
    this->currentTime = 0.0f;
  }
}

bool Timer::IsFinished() { return this->isFinished; }

void Timer::Start() {
  this->started = true;
  this->isFinished = false;

  this->SetDirection(this->direction);
}

void Timer::Pause() { this->started = false; }

void Timer::Resume() { this->started = true; }

void Timer::Stop() {
  this->started = false;
  this->isFinished = false;
  this->timer = 0.0f;

  this->SetDirection(this->direction);
}

void Timer::DispatchTimerFinish() {
  TimerData data = {
      this->currentTime,
      this->maxDuration,
      this->direction,
      this->isFinished,
  };

  EventSystem::GetInstance()->Dispatch(EventType::ON_TIMER_FINISH, &data);
}

void Timer::SetCurrentTime(float time) { this->timer = time; }

float Timer::GetCurrentTime() { return this->timer; }

float Timer::GetRemainingTime() {
  return abs(this->maxDuration - this->currentTime);
}

void Timer::SetDuration(float duration) { this->maxDuration = duration; }

float Timer::GetDuration() { return this->maxDuration; }

void Timer::SetDirection(TimeDirection direction) {
  this->direction = direction;

  if (this->direction == TimeDirection::COUNTDOWN) {
    this->currentTime = this->maxDuration;
  } else {
    this->currentTime = 0.0f;
  }
}

TimeDirection Timer::GetDirection() { return this->direction; }