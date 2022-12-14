#include "entity.hpp"
#include "SFML/Graphics/Texture.hpp"
#include "anim_comp.hpp"
#include <unordered_map>

#include <iostream>

namespace px {
  Entity::Entity(EntityType t) : type_(t) {
    // components_.push_back(new AnimationComponent());
    sprite_.setPosition({500, 1000});
    sprite_.setScale({0.5, 0.5});
    switch (t) {
      case EntityType::Player: {
        sf::Texture walk_texture, run_texture, idle_texture, attack_texture;
        walk_texture.loadFromFile("../res/player/walk.png");
        attack_texture.loadFromFile("../res/player/attack.png");
        idle_texture.loadFromFile("../res/player/idle.png");
        run_texture.loadFromFile("../res/player/run.png");

        sprite_sheets_.emplace("walk", std::move(walk_texture));
        sprite_sheets_.emplace("attack", std::move(attack_texture));
        sprite_sheets_.emplace("idle", std::move(idle_texture));
        sprite_sheets_.emplace("run", std::move(run_texture));
        
        image_counts_.emplace("walk", sf::Vector2u(6, 1));
        image_counts_.emplace("attack", sf::Vector2u(6, 1));
        image_counts_.emplace("idle", sf::Vector2u(4, 1));
        image_counts_.emplace("run", sf::Vector2u(6, 1));

        anim_comp.start(*this);
      } break;
      default: {

      } break;
    }
  }
}