
#include "entt-sfml/entt-sfml.h"

namespace UI::Test
{

  struct TestRegistry
  : entt::registry, UI::RegistryMixin< TestRegistry >
  {

    TestRegistry(sf::RenderWindow* window)
    {
      ui_init(window);
    }
  };


  struct Transform
  {
    double x = 0, y = 0;
    double radians = 0;
  };

  struct Shape
  {
    std::unique_ptr< sf::Shape > shape;
  };

  struct Text
  {
    std::unique_ptr< sf::Text > text;
  };


}

#include <random>

int main()
{
  const char* font_file = "/System/Library/Fonts/Menlo.ttc";

  sf::RenderWindow window;
  window.create(sf::VideoMode(800,600), "test");
  window.setFramerateLimit(60);

  using namespace UI::Test;
  
  TestRegistry registry(&window);

  sf::Color colors[4]{
    sf::Color::Red, sf::Color::Green, sf::Color::Blue, sf::Color::Yellow
  };

  std::default_random_engine random_generator;
  std::uniform_int_distribution< int > color_dist(0,3);
  std::uniform_real_distribution< float > position(0.0, 600.0);
  for(int i = 0; i < 10; ++i)
  {
    auto radius = 30.0;

    auto entity = registry.create();

    registry.assign< Transform >(
      entity,
      position(random_generator),
      position(random_generator),
      0.0 );

    auto sf_shape = std::make_unique< sf::CircleShape >();
    sf_shape->setRadius(radius);
    sf_shape->setOrigin(radius/2, radius/2);
    sf_shape->setFillColor(colors[ color_dist(random_generator) ]);

    registry.assign< Shape >(entity, std::move(sf_shape));
  }

  sf::Font test_font;
  test_font.loadFromFile(font_file);

  {
    auto text_entity = registry.create();
    registry.assign< Transform >(text_entity, 300.0, 280.0, 0.0);
    registry.assign< Text >(
      text_entity,
      std::make_unique< sf::Text >("Hello", test_font, 32.0) );
  }

  while(window.isOpen())
  {
    sf::Event event;
    while(window.pollEvent(event))
    {
      switch(event.type)
      {
      case sf::Event::Closed:
        window.close();
        break;
      case sf::Event::KeyPressed:
        if(event.key.code == sf::Keyboard::Escape)
          window.close();
        break;
      default:
        break;
      }


    }

    // Poll the scene here and enqueue UI::RenderDrawableEvents
    registry.view< Transform, Shape >().each(
      [&](auto entity, auto& transform, auto& shape)
      {
        shape.shape->setPosition(transform.x, transform.y);
        shape.shape->setRotation(transform.radians * 180 / M_PI);
        registry.ctx< entt::dispatcher >()
          .enqueue(UI::RenderDrawableEvent{ shape.shape.get() });
      }
    );
    registry.view< Transform, Text >().each(
      [&](auto entity, auto& transform, auto& text)
      {
        text.text->setPosition(transform.x, transform.y);
        text.text->setRotation(transform.radians * 180 / M_PI);
        registry.ctx< entt::dispatcher >()
          .enqueue(UI::RenderDrawableEvent{ text.text.get() });
      }
    );

    window.clear(sf::Color::Black);
    registry.ctx< entt::dispatcher >().update< UI::RenderDrawableEvent >();
    window.display();
  }

  return 0;

}

