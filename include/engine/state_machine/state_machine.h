#pragma once
#include "engine/ecs/component.h"
#include "engine/ecs/component_metadata.h"
#include "engine/ecs/entity.h"
#include <any>
#include <memory>
#include <string>
#include <unordered_map>

enum class ActionState { RUNNING, SUCCESS, FAILURE, EXIT };
typedef std::string State;

class StateMachineComponent;
class IStateMachineAction;

class StateMachineComponent : public IComponent {
private:
  bool showGraphInspector = false;
  State currentState;
  State initialState;

  std::unordered_map<State, std::unique_ptr<IStateMachineAction>> actions;
  std::unordered_map<std::string, std::any> blackboard;

public:
  static const int ID = ComponentType::STATE_MACHINE;

  StateMachineComponent(IEntity *entity);
  ~StateMachineComponent();

  IEntity *GetEntity() { return this->entity; }

  void Update() override;
  void Draw() override;
  void Debug() override;
  void DrawGraphInspector();

  int GetId() const override { return this->ID; }

  void AddAction(State state, std::unique_ptr<IStateMachineAction> action);

  void TransitionTo(IStateMachineAction *fromState, State toState);

  IStateMachineAction *GetAction(State state);

  void SetInitialState(State state);
  void SetCurrentState(State state);

  State GetCurrentState();
  State GetInitialState();

  template <typename T> void SetVariable(const std::string &key, T value) {
    this->blackboard[key] = std::any(value);
  }

  template <typename T>
  T GetVariable(const std::string &key, T defaultValue = {}) {
    auto it = this->blackboard.find(key);
    if (it != this->blackboard.end()) {
      try {
        return std::any_cast<T>(it->second);
      } catch (const std::bad_any_cast &) {
        return defaultValue;
      }
    }
    return defaultValue;
  }
};
