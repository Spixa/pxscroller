#pragma once
#include "game_engine.hpp"
#include "entity.hpp"
#include "math.h"
#include <iostream>

namespace px {

class Chunk : public px::RenderObject
{
public:
  Chunk(sf::Texture* texture, int posx, int posy) : texture_(texture) { 
    chunk_data_=  new int[chunk_size_.x*chunk_size_.y]{0};

    sf::Vector2i screen;
    screen.x = int(floor( ((posx - posy)) * 16/2));
    screen.y = int(floor( ((posx + posy)) * 16/2));

    std::cout << "Constructed chunk at origin " << screen.x << ", " << screen.y << std::endl;

    origin_.x = screen.x;
    origin_.y = screen.y;

    for (int i = 0; i < 256; i++)
    {
      chunk_data_[i] = 1;
      isometric_sprites_[i].setTexture(*texture); 
    }
  }

  virtual ~Chunk() {
    //delete chunk_data_;
  }

public:
  void draw(sf::RenderTarget& target, sf::RenderStates states) const override
  {
    for (auto x : isometric_sprites_)
      target.draw(x, states);
    return;
  }

  bool set_tile(int at, int material)
  {
    if (at >= 0 && at < 256)
    {
      chunk_data_[at] = material;
      return true;
    } else {
      return false;
    }
  }

  int get_tile_at(int at) {
    if (at >= 0 && at < 256) {
      return chunk_data_[at];
    }
    return -1;
  }

  void start() override
  {
  }

  void update(float dt) override
  {
    auto to_screen = [&](int x, int y) -> sf::Vector2i
    {
      return sf::Vector2i
      {
        (origin_.x * tile_size_.x) + int( floor( ((x - y) * (tile_size_.x)) / 2)),
        (origin_.y * tile_size_.y) + int( floor( ((x + y) * (tile_size_.y)) / 2))
      };
    };
    
    for (int y = 0; y < chunk_size_.y; y++)
    {
      for (int x = 0; x < chunk_size_.x; x++)
      {
        int idx = y * chunk_size_.x + x;
        sf::Vector2i world = to_screen(x, y);

        switch (chunk_data_[idx])
        {
          case 0:
          {
            // invisible
            isometric_sprites_[idx].setTextureRect(sf::IntRect({tile_size_.x, 0}, tile_size_));
            isometric_sprites_[idx].setPosition(world.x, world.y);
          }
          break;
          case 1:
          {
            // grass
            isometric_sprites_[idx].setTextureRect(sf::IntRect({2 *tile_size_.x, 0}, tile_size_));
            isometric_sprites_[idx].setPosition(world.x, world.y);
          }
          break;
          case 2:
          {
            // tree
            isometric_sprites_[idx].setTextureRect(sf::IntRect({0, tile_size_.y}, {tile_size_.x, tile_size_.y * 2}));
            isometric_sprites_[idx].setPosition(world.x, world.y - tile_size_.y);
          }
          break;
          case 3:
          {
            // dead tree
            isometric_sprites_[idx].setTextureRect(sf::IntRect({1 * tile_size_.x, tile_size_.y}, {tile_size_.x, tile_size_.y * 2}));
            isometric_sprites_[idx].setPosition(world.x, world.y - tile_size_.y);
          }
          break;
          case 4:
          {
            // sand
            isometric_sprites_[idx].setTextureRect(sf::IntRect({2 * tile_size_.x, tile_size_.y}, {tile_size_.x, tile_size_.y * 2}));
            isometric_sprites_[idx].setPosition(world.x, world.y - tile_size_.y);
          }
          break;
          case 5:
          {
            isometric_sprites_[idx].setTextureRect(sf::IntRect({3 * tile_size_.x, tile_size_.y}, {tile_size_.x, tile_size_.y * 2}));
            isometric_sprites_[idx].setPosition(world.x, world.y - tile_size_.y);
          }
          break;
          case 6:
          {
            isometric_sprites_[idx].setTextureRect(sf::IntRect({5 * tile_size_.x, 0}, {tile_size_.x, tile_size_.y * 4}));
            isometric_sprites_[idx].setPosition(world.x, world.y - 3 * tile_size_.y);
          }
          break;
        }
      }
    }
  }

  sf::Vector2i get_origin() { return origin_; }
  sf::Vector2i get_tile_size() { return tile_size_; }
  sf::Vector2i get_chunk_size() { return chunk_size_; }

  sf::Sprite* get_sprites()
  {
    return isometric_sprites_;
  }
private:    
  sf::Sprite isometric_sprites_[256];

  sf::Vector2i origin_;
  sf::Vector2i tile_size_{40, 20};
  sf::Vector2i chunk_size_{16, 16};
  int* chunk_data_;

  sf::Texture* texture_;
};


struct Region {
  static constexpr unsigned int REGION_SIZE = 8;
  
  struct pos {
    int x, y;
  };

  explicit Region(std::vector<Chunk>& chunx) {
    if (chunx.size() < REGION_SIZE) 
      throw std::runtime_error("Cannot construct region: Vector provided is too small");
    
    for (int i = 0; i < REGION_SIZE*REGION_SIZE; i++) {
      chunks[i] = &chunx.at(i);
    }
  }

  Chunk* chunks[REGION_SIZE*REGION_SIZE];
};


// https://stackoverflow.com/questions/62916620/c-equivalent-of-javas-math-floormod
inline int floor_mod(int x, int y) {
  x %= y;

  if (x < 0) {
      x += y;
  }

  return x;
}

// https://stackoverflow.com/questions/2622441/c-integer-floor-function
inline int floor_div (int num, int den)
{
  if (0 < (num^den))
    return num/den;
  else
    {
      ldiv_t res = ldiv(num,den);
      return (res.rem)? res.quot-1 
                      : res.quot;
    }
}

class WorldManipulator 
{
  public:
    WorldManipulator()
    {
      cheat_img.loadFromFile("../res/cheat.png");
      selection_img.loadFromFile("../res/highlight.png");
      selection.setTexture(selection_img);

      f.loadFromFile("../res/font.ttf");
      text.setFont(f);
      text.setCharacterSize(14);
      text.setFillColor(sf::Color::Black);
    }

    static void new_sand_region(Region r) {
      sf::Clock cock{};
      for (int i = 0; i < Region::REGION_SIZE*Region::REGION_SIZE; i++) {
        for (int j = 0; j < 256; j++) {

          srand(cock.getElapsedTime().asMicroseconds());
          int d = std::rand() % 3;
          int mat = (d==0 || d==1) ? 1 : 2;
          
          r.chunks[i]->set_tile(j, mat);
        }
      }
    }     

    int get_tile_index(sf::RenderWindow* window, Chunk* chunk, int chunk_idx /* unused */)
    {
      sf::Vector2f mouse = window->mapPixelToCoords(sf::Mouse::getPosition(*window));
      //sf::Vector2i mouse = sf::Vector2i{int(mouse_float.x), int(mouse_float.y)};

      
      sf::Vector2i cell = sf::Vector2i{floor_div(mouse.x, chunk->get_tile_size().x), floor_div(mouse.y,chunk->get_tile_size().y)};
      sf::Vector2i offset = sf::Vector2i{floor_mod(mouse.x, chunk->get_tile_size().x), floor_mod(mouse.y, chunk->get_tile_size().y)};

      sf::Color color;
        
      if (offset.x >= 0 && offset.y >= 0) color = cheat_img.getPixel(offset.x, offset.y);
        
      sf::Vector2i selected =
      {
        (cell.y - chunk->get_origin().y) + (cell.x - chunk->get_origin().x),
        (cell.y - chunk->get_origin().y) - (cell.x - chunk->get_origin().x)  
      };

      if (color == sf::Color::Red) selected += {-1, +0};
      if (color == sf::Color::Blue) selected += {+0, -1};
      if (color == sf::Color::Green) selected += {+0, +1};
      if (color == sf::Color::Yellow) selected += {+1, +0};
      
      int idx = selected.y * chunk->get_chunk_size().x + selected.x;

      auto to_screen = [&](int x, int y) -> sf::Vector2i
      {
        return sf::Vector2i
        {
          (chunk->get_origin().x * chunk->get_tile_size().x) + int( floor( ((x - y) * (chunk->get_tile_size().x)) / 2)),
          (chunk->get_origin().y * chunk->get_tile_size().y) + int( floor( ((x + y) * (chunk->get_tile_size().y)) / 2))
        };
      };

      if (selected.x < chunk->get_chunk_size().x && selected.y < chunk->get_chunk_size().y && selected.x >= 0 && selected.y >= 0)
      {
        selection.setPosition(sf::Vector2f{to_screen(selected.x, selected.y)});
        text.setString("Selected X: " + std::to_string(selected.x) + " - Y: " + std::to_string(selected.y) + "\nMouse position X: " + std::to_string(mouse.x) + " - Y: " + std::to_string(mouse.y));
        return idx;
      }

      return -1;
    }

    int get_tile_under_entity(Entity* e, Chunk* chunk, int chunk_idx /* unused */)
    {
      sf::Vector2f mouse = e->getPosition();
      //sf::Vector2i mouse = sf::Vector2i{int(mouse_float.x), int(mouse_float.y)};

      
      sf::Vector2i cell = sf::Vector2i{floor_div(mouse.x, chunk->get_tile_size().x), floor_div(mouse.y,chunk->get_tile_size().y)};
      sf::Vector2i offset = sf::Vector2i{floor_mod(mouse.x, chunk->get_tile_size().x), floor_mod(mouse.y, chunk->get_tile_size().y)};

      sf::Color color;
        
      if (offset.x >= 0 && offset.y >= 0) color = cheat_img.getPixel(offset.x, offset.y);
        
      sf::Vector2i selected =
      {
        (cell.y - chunk->get_origin().y) + (cell.x - chunk->get_origin().x),
        (cell.y - chunk->get_origin().y) - (cell.x - chunk->get_origin().x)  
      };

      if (color == sf::Color::Red) selected += {-1, +0};
      if (color == sf::Color::Blue) selected += {+0, -1};
      if (color == sf::Color::Green) selected += {+0, +1};
      if (color == sf::Color::Yellow) selected += {+1, +0};
      
      int idx = selected.y * chunk->get_chunk_size().x + selected.x;

      auto to_screen = [&](int x, int y) -> sf::Vector2i
      {
        return sf::Vector2i
        {
          (chunk->get_origin().x * chunk->get_tile_size().x) + int( floor( ((x - y) * (chunk->get_tile_size().x)) / 2)),
          (chunk->get_origin().y * chunk->get_tile_size().y) + int( floor( ((x + y) * (chunk->get_tile_size().y)) / 2))
        };
      };

      if (selected.x < chunk->get_chunk_size().x && selected.y < chunk->get_chunk_size().y && selected.x >= 0 && selected.y >= 0)
      {
        return idx;
      }

      return -1;
    }

    sf::Vector2i to_screen(Chunk* chunk, int x, int y)
    {
      return sf::Vector2i
      {
        (chunk->get_origin().x * chunk->get_tile_size().x) + int( floor( ((x - y) * (chunk->get_tile_size().x)) / 2)),
        (chunk->get_origin().y * chunk->get_tile_size().y) + int( floor( ((x + y) * (chunk->get_tile_size().y)) / 2))
      };
    }



    auto chunk_world_pos(Chunk* chunk)
    {
      return chunk->get_sprites()[0].getPosition();
    }

    void draw_selection(sf::RenderWindow* window)
    {
      // if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
      if (!too_far)
        window->draw(selection);

      text.setPosition(window->getView().getCenter().x - window->getView().getSize().x / 2, window->getView().getCenter().y - window->getView().getSize().y / 2);
      window->draw(text);
    }

    void setSelectionTooFar(bool tof) {
      too_far = tof;
    }
  private:
    sf::Image cheat_img;
    sf::Sprite selection;
    sf::Texture selection_img;

    sf::Text text;
    sf::Font f;
    bool too_far;
};

}