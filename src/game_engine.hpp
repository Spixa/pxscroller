#pragma once

// SFML
#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/RenderStates.hpp"
#include "SFML/Graphics/RenderTarget.hpp"
#include "SFML/Graphics/Texture.hpp"
#include "SFML/Graphics/Transform.hpp"
#include "SFML/Graphics/Transformable.hpp"
#include <SFML/Graphics.hpp>

// std
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <vector>
#include <memory>
#include <string>
#include <utility>
#include <list>
#include <variant>
#include <functional>

namespace px
{
  class Object
  { 
  public:
    Object();
    virtual ~Object();
  
    void set_name(const std::string& name);
    std::string get_name() const;

    void set_property(const std::string& name, const std::variant<int, float, bool, std::string>& property);
    std::variant<int, float, bool, std::string> get_property(const std::string& name) const;

    void set_parent(Object* obj);

    Object* get_parent() const;
    Object* add_object(Object* obj);
    Object* find_obj_by_name(const std::string& name);

    void move_to_back();
    void move_to_front();
    void move_under_to(Object* obj);

    template<typename T>
    T* find_obj_by_name(const std::string& name)
    {
      auto it = std::find_if(objects_.begin(), objects_.end(),
        [this, &name](const Object* obj) -> bool
        {
          return obj->get_name() == name;
        }
      );

      if (it != objects_.end())
        return dynamic_cast<T*>(*it);
      return nullptr;
    }

    template <typename T>
    T* find_obj_by_type()
    {
      for (auto& obj : objects_)
        if (dynamic_cast<T*>(obj) != nullptr)
          return (T*)obj;
      return nullptr;
    }

    template <typename T>
    std::vector<T*> find_objs_by_type()
    {
      std::vector<T*> objects;
      for (auto& obj : objects_)
      {
        if (dynamic_cast<T*>(obj) != nullptr)
          objects.push_back((T*)obj);

        auto objects_temp = obj->find_objs_by_type<T>();
        if (!objects_temp.empty())
          objects.insert(objects.end(), objects_temp.begin(), objects_temp.end());
      }
      return objects;
    }

    template <typename T>
    T* cast_to()
    {
      assert(dynamic_cast<T*>(this));
      return (T*)this;
    }

    template <typename T>
    bool is_type_of() const
    {
      return (dynamic_cast<const T*>(this) != NULL);
    }

    void foreach_object(std::function<void(Object*)> predicate);
    void foreach_object(std::function<void(Object*, bool& need_break)> predicate);

    void remove_object(Object* obj);
    void clear();
    static void invoke_preupdate_actions();

    virtual void start();
    virtual void update(float dt);
    virtual void events(const sf::Event& evnt) {};
    void enable();
    void disable();
    bool is_enabled() const;
    void hide();
    void show();
    bool is_visible() const;

    void turn_on();
    void turn_off();

    const sf::Vector2f get_position() const;
    void set_position(const sf::Vector2f& pos);
    void set_position(float x, float y);
    void move(const sf::Vector2f& point);
    virtual sf::FloatRect get_bounds() const;
    virtual void set_bounds(const sf::FloatRect& rect);
    void set_size(const sf::Vector2f& size);
  protected:
    virtual void onPropertySet(const std::string& name);
    virtual void onPropertyGet(const std::string& name) const;
    virtual void onActivated() {};
    virtual void onPositionChanged(const sf::Vector2f& new_pos, const sf::Vector2f& old_pos) {};

  private:
    std::string name_;
    bool started_ = false;
    static std::vector<std::function<void()>> preupdate_actions_;
    std::map<std::string, std::variant<int, float, bool, std::string>> properties_;
    Object* parent_;
    std::list<Object*> objects_;
    bool enable_;
    bool visible_;
    sf::Vector2f position_, size_;

  };

  class RenderObject : public Object, public sf::Drawable, public sf::Transformable {};


  class Game
  {
  public:
    Game(std::string const& title, sf::Vector2u window_size);
    virtual ~Game() {}

    int exec();
  protected:
    sf::String title_;
    uint32_t width_, height_;
    sf::Color clear_color_;
    sf::Clock dt_clock_{};
    float dt_{};

    float zoom_;
    sf::View view_{};

    std::unique_ptr<sf::RenderWindow> window_;
  private:
    void init();
    void loop();
    void render();
    void update();
  private: // init
    virtual void dInit() = 0;
  private: // render
    virtual void dRender() = 0;
  private: // update
    void update_sfml_events();
    virtual void dUpdate() = 0;
    void update_delta_time();
  };
};