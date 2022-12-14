#include "anim_comp.hpp"
#include "entity.hpp"
namespace px {

void AnimationComponent::start(Entity& e) {
  selected = "idle";
  animRect.width = textures[selected].getSize().x / float(imageCount[selected].x);
  animRect.height = textures[selected].getSize().y  / float(imageCount[selected].y);

  e.setSpriteTexture(textures[selected]);
}

void AnimationComponent::update(Entity& e, float deltaTime) {
  currentImage.y = 0;
  totalTime+= deltaTime;

  if (totalTime >= switchTime) {
    totalTime -= switchTime;
    currentImage.x++;

    if (currentImage.x >= imageCount[selected].x) {
      currentImage.x = 0;
    }
  }

  animRect.left = currentImage.x * animRect.width;
  animRect.top = currentImage.y * animRect.height;

  e.setSpriteTextureRect(animRect);
  e.setOrigin({float(animRect.width / 2), float(animRect.height)});
  e.setSpriteTexture(textures[selected]);
}

void AnimationComponent::request(AnimationRequests r) {
  switch (r) {
    case AnimationRequests::SetAttackAnimation: {
      selected = "attack";
    } break;
    case AnimationRequests::SetIdleAnimation: {
      selected = "idle";
    } break;
    case AnimationRequests::SetRunAnimation: {
      selected = "run";
    } break;
    case AnimationRequests::SetWalkAnimation: {
      selected = "walk";
    } break;
  }
}

}