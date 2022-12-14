#pragma once
#include "game_engine.hpp"

namespace px {
  template<typename T, typename R>
  class Component {
  public:
    Component(const std::string& name) 
      : name_(name) {}
    virtual void start(T& t) = 0;
    virtual void update(T& t, float deltaTime) = 0;

    const std::string& get_name() const { return name_; }
    virtual void request(R r) = 0;
  private:
    std::string name_;
  };
}