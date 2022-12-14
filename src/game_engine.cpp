#include "game_engine.hpp"
#include "SFML/Window/Keyboard.hpp"

// std
#include <SFML/Window/WindowStyle.hpp>
#include <iostream>
#include <cassert>

namespace px 
{
  const sf::Vector2f Object::get_position() const
  {
      return position_;
  }

  void Object::set_position(const sf::Vector2f& point)
  {
      onPositionChanged(point, position_);
      position_ = point;
  }

  void Object::set_position(float x, float y)
  {
      set_position(sf::Vector2f(x, y));
  }

  void Object::move(const sf::Vector2f& point)
  {
      position_ += point;
  }

  void Object::set_size(const sf::Vector2f& size)
  {
      size_ = size;
  }

  sf::FloatRect Object::get_bounds() const
  {
      return sf::FloatRect(position_,size_);
  }

  void Object::set_bounds(const sf::FloatRect& rect)
  {
      position_ = {rect.left, rect.top};
      size_ = {rect.width, rect.height};
  }

  Object::Object()
  {
      enable_ = visible_ = true;
      parent_ = NULL;
  }

  void Object::set_parent(Object* game_object)
  {
      parent_ = game_object;
  }

  Object* Object::get_parent() const
  {
      return parent_;
  }

  void Object::update(float delta_time)
  {
      if (is_enabled())
      {
          for (auto& obj : objects_)
              if (!obj->started_)
              {
                  obj->started_ = true;
                  obj->start();
              }

          for (auto& obj : objects_)
              if (obj->is_enabled())
                  obj->update(delta_time);
      }
  }

  void Object::start()
  {

  }

  void Object::set_name(const std::string& name)
  {
      name_ = name;
  }

  std::string Object::get_name() const
  {
      return name_;
  }

  void Object::set_property(const std::string& name, const std::variant<int, float, bool, std::string>& property)
  {
      properties_[name] = property;
      onPropertySet(name);
  };

  std::variant<int, float, bool, std::string> Object::get_property(const std::string& name) const
  {
      onPropertyGet(name);
      return const_cast<Object*>(this)->properties_[name];
  };

  void Object::disable()
  {
      enable_ = false;
  }

  void Object::enable()
  {
      enable_ = true;
  }

  bool Object::is_enabled() const
  {
      return enable_;
  }

  void Object::hide()
  {
      visible_ = false;
  }

  void Object::show()
  {
      visible_ = true;
  }

  bool Object::is_visible() const
  {
      return visible_;
  }

  void Object::turn_on()
  {
      show();
      enable();
  }
  void Object::turn_off()
  {
      hide();
      disable();
  }

  Object* Object::add_object(Object* object)
  {
      objects_.push_back(object);
      object->set_parent(this);
      object->onActivated();
      if (started_)
      {
          object->started_ = true;
          object->start();
      }
      return object;
  }

  Object* Object::find_obj_by_name(const std::string& name)
  {
      auto it = std::find_if(objects_.begin(), objects_.end(), [this, &name](const Object* obj) -> bool { return obj->get_name() == name;  });
      if (it != objects_.end())
          return *it;
      return nullptr;
  }

  Object::~Object()
  {
      for (auto& obj : objects_)
          delete obj;
      objects_.clear();
  }

  void Object::foreach_object(std::function<void(Object*)> predicate)
  {
      for (auto& obj : objects_)
          predicate(obj);
  }
  void Object::foreach_object(std::function<void(Object*, bool& )> predicate)
  {
      bool need_break = false;
      for (auto& obj : objects_)
      {
          predicate(obj, need_break);
          if (need_break)
              break;
      }
  }

  void Object::remove_object(Object* object)
  {
      auto action = [this, object]() //remove object later
      {
          auto it = std::find(objects_.begin(), objects_.end(), object);
          assert(it != objects_.end());
          objects_.erase(it);
          delete object;
      };
      preupdate_actions_.push_back(action);
  }

  void Object::onPropertySet(const std::string& name)
  {
      if (name == "x")
          set_position(std::get<float>(properties_["x"]),get_position().y);
      else if (name == "y")
          set_position(get_position().x, std::get<float>(properties_["y"]));
      else if (name == "name")
          set_name(std::get<std::string>(properties_["name"]));

  }

  void Object::onPropertyGet(const std::string& name) const
  {

  }

  void Object::move_to_back()
  {
      if (get_parent())
      {
          auto move_to_back_action = [this]()
          {
              auto list = &(get_parent()->objects_);
              auto it = std::find(list->begin(), list->end(), this);
              assert(*it == this);
              auto tmp = *it;
              it = list->erase(it);
              list->push_front(tmp);
          };
          preupdate_actions_.push_back(move_to_back_action);
      }
  }

  void Object::move_to_front()
  {
      if (get_parent())
      {
          auto move_to_front_action = [this]()
          {
              auto list = &(get_parent()->objects_);
              auto it = std::find(list->begin(), list->end(), this);
              assert(*it == this);
              auto tmp = *it;
              it = list->erase(it);
              list->push_back(tmp);
          };

          preupdate_actions_.push_back(move_to_front_action);
      }
  }

  void Object::move_under_to(Object* obj)
  {
      if (get_parent())
      {
          auto move_under_action = [this,obj]()
          {
              auto list = &(get_parent()->objects_);
              auto this_obj = std::find(list->begin(), list->end(), this);
              auto other_obj = std::find(list->begin(), list->end(), obj);
              assert(this_obj != list->end() && other_obj != list->end());
              list->erase(this_obj);
              list->insert(other_obj,this);
          };

          preupdate_actions_.push_back(move_under_action);
      }
  }

  void Object::clear()
  {
      for (auto object : objects_)
          delete object;
      objects_.clear();
  }

  std::vector<std::function<void()>> Object::preupdate_actions_ = std::vector<std::function<void()>>();

  void Object::invoke_preupdate_actions()
  {
      for (auto& action : preupdate_actions_)
          action();
      preupdate_actions_.clear();
  }



  //////////////////////////////////////////////////////////
  //    GAME CLASS IMPLEMENTION                         ///
  /////////////////////////////////////////////////////////
  Game::Game(std::string const& title, sf::Vector2u window_size)
    : title_(title), width_(window_size.x), height_(window_size.y),
      clear_color_(255, 255, 255, 255),
      window_(std::make_unique<sf::RenderWindow>(sf::VideoMode(width_, height_), title_, sf::Style::Titlebar | sf::Style::Close | sf::Style::Resize))
  {
    std::cout << "window creation: done\n"; 
  }

  int Game::exec()
  {
    init();
    loop();
    return 0;

  }

  void Game::init()
  {
      view_ = sf::View(sf::FloatRect({0 , 0}, sf::Vector2f{window_->getSize()}));


      dInit();
  }

  void Game::loop()
  {
    while (window_->isOpen())
    {
      render();
      update();
    }
  }

  void Game::render()
  {
    window_->clear(clear_color_);

    dRender();

    window_->display();
  }

  void Game::update()
  {
    update_sfml_events();

    dUpdate();    
    window_->setView(view_);
    // this has to be dead last
    update_delta_time();
  }

  void Game::update_sfml_events()
  {
    using sf::Event;
    Event evnt;
    while (window_->pollEvent(evnt))
    {
      switch (evnt.type)
      {
        case Event::Closed: 
        {
          std::cout << "Goodbye! \n";
          window_->close();
        }
        break;
        case Event::Resized:
        {
          std::cout << "Window resized: ";
          std::cout << window_->getSize().x << " - " << window_->getSize().y << std::endl;
        }
        break;
        case Event::KeyPressed:
            // if (evnt.key.code == sf::Keyboard::O)
            // {
            //     zoom_/=2;
            //     view_.zoom(zoom_);
            // } else if (evnt.key.code == sf::Keyboard::I)
            // {
            //     zoom_*=2;
            //     view_.zoom(zoom_);
            // }
        default:
        {

        }
        break;
      }
    }
  }

  void Game::update_delta_time()
  {
    dt_ = dt_clock_.restart().asSeconds();
  }

};