#include "engine/ecs/entity.h"

enum TimeDirection { COUNTDOWN, COUNTUP };

struct TimerData {
  float currentTime;
  float maxDuration;
  TimeDirection direction;
  bool isFinished;
};

class Timer : public IEntity {
public:
  Timer(float duration, TimeDirection direction = COUNTDOWN, int precision = 2,
        int fontSize = 20);
  ~Timer();

  bool IsFinished();
  void SetCurrentTime(float time);
  float GetCurrentTime();
  float GetRemainingTime();
  void SetDuration(float duration);
  float GetDuration();
  void SetDirection(TimeDirection direction);
  void AddTime(float time);
  void RemoveTime(float time);
  TimeDirection GetDirection();

  void Start();
  void Pause();
  void Resume();
  void Stop();

  void Update() override;
  void Draw() override;
  void Debug() override;

private:
  float timer;

  float currentTime = 0.0f;
  float maxDuration;

  bool isFinished = false;
  bool started = false;

  TimeDirection direction;

  int precision;
  int fontSize;

  void DispatchTimerFinish();
};