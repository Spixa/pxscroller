#pragma once
#include "SFML/Window/Keyboard.hpp"
#include "SFML/Window/Mouse.hpp"
#include "comp.hpp"
#include "game_engine.hpp"
#include "anim_comp.hpp"

namespace px {

enum class EntityType {
  Player
};

class Entity : public px::RenderObject {
public:
  Entity(EntityType t);
  virtual ~Entity() {
  }
public:
  void update(float dt) {
    is_attacking_ = false;
    anim_comp.update(*this, dt);
    
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && attackCooldown.getElapsedTime().asSeconds() > 0.5) {
      attackCooldown.restart();
      attackProgress.restart();
      is_attacking_ = true;
      anim_comp.request(AnimationRequests::SetAttackAnimation);
    } 
    
    else if (!sf::Mouse::isButtonPressed(sf::Mouse::Left) && attackProgress.getElapsedTime().asSeconds() > 0.5) {
      anim_comp.request(AnimationRequests::SetIdleAnimation);
      attackProgress.restart();
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
        
          anim_comp.request(AnimationRequests::SetWalkAnimation);
    } else {
        if (anim_comp.get_selected() != "attack")
          anim_comp.request(AnimationRequests::SetIdleAnimation);
    }   
  }
  void setSpriteTexture(sf::Texture& t) { sprite_.setTexture(t); }

  void setSpriteTextureRect(sf::IntRect const& rect) { sprite_.setTextureRect(rect); }

  void setSpritePosition(sf::Vector2f vec) { sprite_.setPosition(vec); }

  const sf::Vector2f getPosition() const { return sprite_.getPosition(); }
  void setOrigin(sf::Vector2f origin) { return sprite_.setOrigin(origin); }

  bool isAttacking() const { return is_attacking_; }
private:
  void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
    states.transform *= getTransform();
    target.draw(sprite_, states);
  }
private:
  EntityType type_;
  sf::Sprite sprite_;
  std::unordered_map<std::string, sf::Texture> sprite_sheets_;
  std::unordered_map<std::string, sf::Vector2u> image_counts_;
  bool is_attacking_ = false;

  sf::Clock attackCooldown{};
  sf::Clock attackProgress{};
  AnimationComponent anim_comp{sprite_sheets_, image_counts_, 0.1};
};
}