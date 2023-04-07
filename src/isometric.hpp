#pragma once

#include <cstddef>
#include <cstdlib>
#include <exception>
#include <random>
#include "game_engine.hpp"
#include <SFML/Graphics.hpp>
#include <stdexcept>
#include <string>
#include <fstream>
#include <iostream>
#include "math.h"
#include "entity.hpp"

#include <optional>

#include "chunk.hpp"

namespace px
{
  class IsometricMap : public px::Game, public sf::NonCopyable
  {
  public:
    IsometricMap() : Game("Hello World", {1280,720}), e(EntityType::Player)
    {
      text.loadFromFile("../res/iso.png");
    }

    ~IsometricMap()
    {

    }


  private:
    int selected_tile_id = 0;
    sf::Texture text;
    std::vector<Chunk> chunks;
    WorldManipulator manip_{};
    bool zoomed = false;

    Entity e;
  protected:
    void dInit() override
    {
      for (int y = 0; y < 8; y++)
      {
        for (int x = 0; x < 8; x++)
        {
          chunks.push_back(Chunk(&text, x, y));
        }
      }

      Region r{chunks};
      

      WorldManipulator::new_sand_region(r);  

      for (auto&x : chunks)
      {
        x.start();
      }

      view_.setCenter(manip_.chunk_world_pos(&chunks[0]));
    }

    void dUpdate() override
    {
      e.update(dt_);

      if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z) && !zoomed) {
        view_.zoom(0.3);
        zoomed = true;
      }

      std::size_t idx{};
      for (auto& x : chunks)
      {
        x.update(dt_);
        int entity_tile = manip_.get_tile_under_entity(&e, &x, idx);
        int selected_tile = manip_.get_tile_index(window_.get(), &x, idx);

        if (entity_tile == -1) continue;

        if (e.isAttacking()) {
          x.set_tile(selected_tile, 1);
        }
        
        idx++;
      }

      float speed = 300.0;

      if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
      {
        view_.move({-speed * dt_, 0});
      }

      if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
      {
        view_.move({speed * dt_, 0});
      }

      if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
      {
        view_.move({0, -speed * dt_});
      }

      if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
      {
        view_.move({0, speed * dt_});;
      }

      if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num0))
      {
        selected_tile_id = 0;
      } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1))
      {
        selected_tile_id = 1;
      } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2))
      {
        selected_tile_id = 2;
      } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num3))
      {
        selected_tile_id = 3;
      } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num4))
      {
        selected_tile_id = 4;
      } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num5))
      {
        selected_tile_id = 5;
      } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num6))
      {
        selected_tile_id = 6;
      }

      e.setSpritePosition(window_->getView().getCenter());

    }

    void dRender() override
    {
      for (auto& x : chunks)
      {
        window_->draw(x);
      }
      manip_.draw_selection(window_.get());
      window_->draw(e);
    }

  };
};
