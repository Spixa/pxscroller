#pragma once
#include <vector>
#include "states.hpp"

namespace px {

enum class States : unsigned long {
  MainMenuState = 0,
  InGameState = 1,
};

class StateManager {
private:
  std::vector<State*> states_;
  State* running_state_;
  States running_state_type_;
public:
  StateManager() {
    states_.push_back(new states::InGameState());

    running_state_ = states_[0];
    running_state_type_ = States::InGameState;
  }

  void render(sf::RenderWindow* window) {
    if (running_state_) {
      running_state_->render(window);
    }
  }

  void update(float dT, sf::RenderWindow* window) {
    if (running_state_) {
      running_state_->update(dT, window);
    }
  }

  void update_event(sf::Event event) { 
    if (running_state_) {
      running_state_->update_events(event);
    }
  }

  States get_current() { 
    return running_state_type_;
  }

  void loadScene(States const& state) {
    if (state == running_state_type_) return;

    switch (state) {
      case States::MainMenuState: {

      } break;
      case States::InGameState: {
        running_state_ = states_[static_cast<unsigned long>(States::InGameState)];
        running_state_type_ = States::InGameState;
      }
    }
  }
};

}