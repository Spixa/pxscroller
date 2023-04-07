#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/System/NonCopyable.hpp>
#include <vector>
#include <memory> 
#include <functional>

namespace px {
  class State : public sf::NonCopyable {
      std::vector<std::shared_ptr<sf::Drawable>> drawables;  
      sf::String name;
      std::vector<std::function<void( sf::Event)>> fns;

    protected:
      void registerEvent(std::function<void(sf::Event)> callback) {
        fns.push_back(callback);
      }


    public:
      explicit State(sf::String const& name) : name(name) {

      }

      void pushDrawable(std::shared_ptr<sf::Drawable> drawable) {
        drawables.push_back(drawable);
      }

      void render(sf::RenderWindow* window) {
        for (auto x : drawables) {
          window->draw(*x.get());
        }
      }

      virtual void update(float deltaTime, sf::RenderWindow* window) = 0;
      void update_events(sf::Event event) {
        for (auto x : fns) {
          x(event);
        }
      }
  };

  namespace states {
    class InGameState : public px::State {
    public:
      InGameState() : State("ing") {

      }

      void update(float deltaTime, sf::RenderWindow* window) override {
        
      }
    };
  }
}