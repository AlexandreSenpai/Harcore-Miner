#pragma once
#include "engine/state_machine/state_machine.h"

class IStateMachineAction {
protected:
  StateMachineComponent *stateMachine;

public:
  IStateMachineAction(StateMachineComponent *sm);

  ActionState state;
  State nextState;

  virtual ActionState OnEnter() = 0;
  virtual ActionState OnUpdate() = 0;
  virtual ActionState OnExit() = 0;

  virtual void Debug() {}

  virtual std::vector<State> GetPossibleTransitions() {
    return {this->nextState};
  }
};
