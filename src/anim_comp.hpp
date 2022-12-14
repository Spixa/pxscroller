#pragma once
#include "SFML/Graphics/Texture.hpp"
#include "comp.hpp"
#include <string>

namespace px {
  enum class AnimationRequests {
    SetWalkAnimation = 0,
    SetRunAnimation = 1,
    SetIdleAnimation = 2,
    SetAttackAnimation = 3
  };

  class Entity;
  class AnimationComponent : public Component<Entity, AnimationRequests> {
  public:
    AnimationComponent(std::unordered_map<std::string, sf::Texture>& textures, std::unordered_map<std::string, sf::Vector2u>& imageCounts, float switchTime) : Component<Entity, AnimationRequests>("animation"), textures(textures), imageCount(imageCounts), switchTime(switchTime) {}

    void start(Entity& t) override;
    void update(Entity& t, float deltaTime) override;
    void request(AnimationRequests r) override;
    std::string get_selected() const { return selected; }
  private:
    std::string selected{"idle"};
    std::unordered_map<std::string, sf::Vector2u>& imageCount;
    std::unordered_map<std::string, sf::Texture>& textures;

    sf::Vector2u currentImage;
    sf::IntRect animRect;
    float switchTime;
    float totalTime;
  };
}