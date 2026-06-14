#include "engine/state_machine/state_machine.h"
#include "engine/ecs/entity.h"
#include "engine/state_machine/action.h"
#include <imgui.h>
#include <queue>


IStateMachineAction::IStateMachineAction(StateMachineComponent *stateMachine)
    : stateMachine(stateMachine) {}

StateMachineComponent::StateMachineComponent(IEntity *entity)
    : IComponent(entity, "StateMachine") {
  this->currentState = "ROOT";
}

StateMachineComponent::~StateMachineComponent() {}

void StateMachineComponent::Update() {
  if (!this->IsEnabled())
    return;

  State current = this->GetCurrentState();
  IStateMachineAction *action = this->GetAction(current);

  if (!action)
    return;

  if (this->currentState == this->initialState) {
    action->OnEnter();
  }

  ActionState result = action->OnUpdate();

  if (result == ActionState::SUCCESS) {
    this->TransitionTo(action, action->nextState);
  } else if (result == ActionState::FAILURE || result == ActionState::EXIT) {
    this->TransitionTo(action, this->initialState);
  }
}

void StateMachineComponent::Draw() {}

void StateMachineComponent::Debug() {
  IComponent::Debug(); // Call base debug

  ImGui::Text("Current State: %s", this->currentState.c_str());
  ImGui::Text("Initial State: %s", this->initialState.c_str());

  if (ImGui::Button("Open Graph Inspector")) {
    this->showGraphInspector = true;
  }

  ImGui::SeparatorText("Actions");
  for (const auto &pair : this->actions) {
    ImGui::Text("%s", pair.first.c_str());
  }

  ImGui::SeparatorText("Current Action Properties");
  IStateMachineAction *currentAction = this->GetAction(this->currentState);
  if (currentAction != nullptr) {
    currentAction->Debug();
  } else {
    ImGui::TextDisabled("No active action.");
  }

  ImGui::SeparatorText("Blackboard Keys");
  for (const auto &pair : this->blackboard) {
    ImGui::Text("%s", pair.first.c_str());
  }

  if (this->showGraphInspector) {
    this->DrawGraphInspector();
  }
}

void StateMachineComponent::TransitionTo(IStateMachineAction *fromState,
                                         State toState) {
  fromState->OnExit();

  this->SetCurrentState(toState);

  IStateMachineAction *action = this->GetAction(toState);

  if (action == nullptr) {
    return;
  }

  action->OnEnter();
}

IStateMachineAction *StateMachineComponent::GetAction(State state) {
  auto action = this->actions.find(state);

  if (action == this->actions.end()) {
    return nullptr;
  }

  return action->second.get();
}

void StateMachineComponent::AddAction(
    State state, std::unique_ptr<IStateMachineAction> action) {
  this->actions[state] = std::move(action);
}

void StateMachineComponent::DrawGraphInspector() {
  std::string windowTitle =
      "State Machine Graph: " + std::string(this->entity->GetName()) + "###" +
      this->entity->GetId();
  ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_FirstUseEver);

  if (!ImGui::Begin(windowTitle.c_str(), &this->showGraphInspector)) {
    ImGui::End();
    return;
  }

  ImDrawList *drawList = ImGui::GetWindowDrawList();
  ImVec2 canvasP0 = ImGui::GetCursorScreenPos();
  ImVec2 canvasSz = ImGui::GetContentRegionAvail();
  if (canvasSz.x < 50.0f)
    canvasSz.x = 50.0f;
  if (canvasSz.y < 50.0f)
    canvasSz.y = 50.0f;
  ImVec2 canvasP1 = ImVec2(canvasP0.x + canvasSz.x, canvasP0.y + canvasSz.y);

  // Draw border and background
  drawList->AddRectFilled(canvasP0, canvasP1, IM_COL32(50, 50, 50, 255));
  drawList->AddRect(canvasP0, canvasP1, IM_COL32(255, 255, 255, 255));

  ImGui::SetCursorScreenPos(ImVec2(canvasP0.x + 10, canvasP0.y + 10));
  ImGui::Text("Actions: %zu", this->actions.size());

  // This will pan/zoom eventually, but for now we do simple layout.
  // We'll map each state to a fixed 2D coordinate for simplicity.
  // Determine Depths using BFS
  std::unordered_map<State, int> nodeDepths;
  std::unordered_map<int, std::vector<State>> depthLayers;
  std::queue<std::pair<State, int>> q;

  q.push({this->initialState, 0});
  nodeDepths[this->initialState] = 0;

  while (!q.empty()) {
    auto [current, depth] = q.front();
    q.pop();

    IStateMachineAction *action = this->GetAction(current);
    if (action) {
      std::vector<State> nextStates = action->GetPossibleTransitions();
      for (const State &next : nextStates) {
        State target = next.empty() ? this->initialState : next;

        // BFS: shortest path depth
        if (nodeDepths.find(target) == nodeDepths.end()) {
          nodeDepths[target] = depth + 1;
          q.push({target, depth + 1});
        }
      }
    }
  }

  std::unordered_map<State, ImVec2> nodePositions;

  // Handle any orphan nodes that aren't reachable from initialState
  for (const auto &pair : this->actions) {
    if (nodePositions.find(pair.first) == nodePositions.end() &&
        nodeDepths.find(pair.first) == nodeDepths.end()) {
      nodeDepths[pair.first] = 0;
    }
  }

  int maxDepth = 0;
  for (const auto &pair : nodeDepths) {
    depthLayers[pair.second].push_back(pair.first);
    if (pair.second > maxDepth)
      maxDepth = pair.second;
  }

  // Calculate Positions Top-Down
  float nodeWidth = 200.0f;
  float nodeHeight = 40.0f;
  float xSpacing = 40.0f;
  float ySpacing = 80.0f;

  for (int depth = 0; depth <= maxDepth; depth++) {
    if (depthLayers.find(depth) == depthLayers.end())
      continue;
    const auto &layerNodes = depthLayers[depth];

    float dy = 50.0f + depth * (nodeHeight + ySpacing);

    // Center nodes horizontally
    float totalWidth =
        layerNodes.size() * nodeWidth + (layerNodes.size() - 1) * xSpacing;
    float startX = canvasP0.x + (canvasSz.x - totalWidth) / 2.0f;

    for (size_t i = 0; i < layerNodes.size(); i++) {
      float dx = startX + i * (nodeWidth + xSpacing);
      nodePositions[layerNodes[i]] = ImVec2(dx, canvasP0.y + dy);
    }
  }

  // Draw connections first (so they are under the nodes)
  for (const auto &pair : this->actions) {
    State from = pair.first;
    std::vector<State> transitions = pair.second->GetPossibleTransitions();

    for (State to : transitions) {
      if (to.empty() || to == this->initialState)
        continue;

      if (nodePositions.find(from) != nodePositions.end() &&
          nodePositions.find(to) != nodePositions.end()) {

        ImVec2 p1 = nodePositions[from];
        ImVec2 p2 = nodePositions[to];

        // Bottom center of parent
        p1.x += nodeWidth / 2.0f;
        p1.y += nodeHeight;

        // Top center of child
        p2.x += nodeWidth / 2.0f;

        ImVec2 cp1 = ImVec2(p1.x, p1.y + 40.0f);
        ImVec2 cp2 = ImVec2(p2.x, p2.y - 40.0f);

        // For backward edges, route line out to the side
        if (p2.y <= p1.y) {
          cp1 = ImVec2(p1.x - 120.0f, p1.y + 40.0f);
          cp2 = ImVec2(p2.x - 120.0f, p2.y - 40.0f);
        }

        // Draw bezier looking curve
        drawList->AddBezierCubic(p1, cp1, cp2, p2, IM_COL32(200, 200, 200, 255),
                                 2.0f);
      }
    }
  }

  // Draw nodes
  for (const auto &pair : this->actions) {
    State stateName = pair.first;
    ImVec2 pos = nodePositions[stateName];
    ImVec2 size(nodeWidth, nodeHeight);
    ImVec2 p2(pos.x + size.x, pos.y + size.y);

    // Highlight if active
    ImU32 bgColor = (stateName == this->currentState)
                        ? IM_COL32(0, 150, 0, 255)
                        : IM_COL32(100, 100, 100, 255);

    drawList->AddRectFilled(pos, p2, bgColor, 4.0f);
    drawList->AddRect(pos, p2, IM_COL32(255, 255, 255, 255), 4.0f);

    // Center text somewhat manually
    ImVec2 textSize = ImGui::CalcTextSize(stateName.c_str());
    ImVec2 textPos(pos.x + (size.x - textSize.x) * 0.5f,
                   pos.y + (size.y - textSize.y) * 0.5f);
    drawList->AddText(textPos, IM_COL32(255, 255, 255, 255), stateName.c_str());
  }

  ImGui::End();
}

State StateMachineComponent::GetCurrentState() { return this->currentState; }

State StateMachineComponent::GetInitialState() { return this->initialState; }

void StateMachineComponent::SetCurrentState(State state) {
  this->currentState = state;
}

void StateMachineComponent::SetInitialState(State state) {
  this->initialState = state;
}