
#include "entt-mruby/entt-mruby.h"

#include "entt-sfml/entt-sfml.h"
#include "entt-sfml/asset-cache.h"

#include "entt-box2d/entt-box2d.h"

#include <mruby/hash.h>
#include <mruby/numeric.h>
#include <mruby/string.h>

#include "components/shape.h"
#include "components/text.h"
#include "components/transform.h"
#include "components/sprite.h"
#include "components/style.h"

#include "entt-sfml/controllers/keyboard.h"
#include "renderer.h"

struct TestRegistry
: entt::registry,
  MRuby::RegistryMixin< TestRegistry >,
  UI::RegistryMixin< TestRegistry >,
  Physics::RegistryMixin< TestRegistry >
{
  static const int max_static_components;
  int next_dynamic_component_id = max_static_components;

  mrb_state* mrb;

  TestRegistry(sf::RenderWindow* window, const std::string& script_file)
  {
    mrb = set< mrb_state* >(mrb_open());
    mrb_init<
      Transform, Text, Sprite, Shape, Style,
      Physics::World, Physics::Body, Physics::Fixture
    >(mrb);

    ui_init(window);
    ui_mrb_init(mrb);

    if(window)
    {
      SfmlRenderer2D& renderer = set< SfmlRenderer2D >(*window);
      renderer.render_options.fill_color = sf::Color::White;
      ui_mrb_init_renderer(mrb);
    }

    physics_init();
    physics_mrb_init(mrb);

    mrb_load_file(mrb, script_file.c_str());
  }

  void ui_mrb_init_renderer(mrb_state* state)
  {
    // create a class to interact with the renderer
    if constexpr(std::is_base_of_v< MRuby::RegistryMixin<TestRegistry>, TestRegistry >)
    {
      // Set up an interface to access the registry from ruby
      auto renderer_class = MRuby::Class::bind< SfmlRenderer2D* >(
        state, "Renderer", state->object_class);

      renderer_class
        .define_method("rectangle", &TestRegistry::ui_mrb_renderer_rect, MRB_ARGS_REQ(4))
      ;

      auto registry_class = MRuby::Class{ state, mrb_class_get(state, "Registry") };
      registry_class
        .define_method("renderer", &TestRegistry::ui_mrb_registry_renderer, MRB_ARGS_REQ(0))
      ;
    }
  }

  using MRubyRendererPtr = SfmlRenderer2D*;
  static SfmlRenderer2D* mrb_value_to_renderer(mrb_state* state, mrb_value value)
  {
    auto p = DATA_CHECK_GET_PTR(state, value, &::MRuby::DefaultClassBinder< MRubyRendererPtr >::mrb_type, MRubyRendererPtr);
    if(!p)
    {
      std::cout << "mrb_value_to_renderer data ptr is null" << std::endl;
      return nullptr;
    }
    return *p;
  }

  static mrb_value ui_mrb_renderer_rect(mrb_state* state, mrb_value self)
  {
    SfmlRenderer2D* renderer = mrb_value_to_renderer(state, self);
    if(!renderer)
      return mrb_nil_value();

    mrb_float left, top, width, height, angle = 0;
    if(mrb_get_args(state, "ffff|f", &left, &top, &width, &height, &angle) < 4)
      return mrb_nil_value();

    renderer->rectangle(Transform{ left, top, angle }, width, height);
    return self;
  }

  static mrb_value ui_mrb_registry_renderer(mrb_state* state, mrb_value self)
  {
    TestRegistry* registry = mrb_value_to_registry(state, self);
    if(!registry)
      return mrb_nil_value();

    auto renderer_class = MRuby::Class{ state, mrb_class_get(state, "Renderer") };
    auto renderer_obj = renderer_class.new_(0, nullptr);
    auto renderer_data = (MRubyRendererPtr*)DATA_PTR(renderer_obj);
    *renderer_data = &registry->renderer();

    return renderer_obj;
  }

  SfmlRenderer2D& renderer()
  {
    return ctx< SfmlRenderer2D >();
  }
};

const int TestRegistry::max_static_components = 16;

#include <random>

int main(int argc, const char** argv)
{
  std::string script_file = "assets/game.rb";
  if(argc == 2)
    script_file = argv[1];

  sf::RenderWindow window;
  window.create(sf::VideoMode(800,600), "test");
  window.setFramerateLimit(60);
  
  TestRegistry registry(&window, script_file);

  auto last_tick = std::chrono::high_resolution_clock::now();

  while(window.isOpen())
  {
    // Handle events
    sf::Event event;
    while(window.pollEvent(event))
    {
      switch(event.type)
      {
      case sf::Event::Closed:
        window.close();
        break;
      default:
        break;
      }

    }

    // Update stage
    auto time_now = std::chrono::high_resolution_clock::now();
    auto time_delta = std::chrono::duration_cast< std::chrono::milliseconds >(
      time_now - last_tick );
    last_tick = time_now;

    registry.ui_controller_manager().update(registry, time_delta); // std::chrono::milliseconds(int(dt*1000)));
    registry.ctx< entt::dispatcher >().update< UI::ControllerInputEvent >();

    window.clear(sf::Color::Black);

    mrb_funcall(
      registry.mrb,
      mrb_top_self(registry.mrb),
      "update",
      1,
      mrb_fixnum_value(time_delta.count()));
    if(registry.mrb->exc)
    {
      mrb_print_error(registry.mrb);
      mrb_print_backtrace(registry.mrb);
      window.close();
      continue;
    }

    // Poll the scene here and enqueue UI::RenderDrawableEvents
    // registry.view< Transform, Shape >().each(
    //   [&](auto entity, auto& transform, auto& shape)
    //   {
    //     shape.shape->setPosition(transform.x, transform.y);
    //     shape.shape->setRotation(transform.radians * 180 / M_PI);
    //     registry.ctx< entt::dispatcher >()
    //       .enqueue(UI::RenderDrawableEvent{ shape.shape.get() });
    //   }
    // );

    // registry.view< Transform, Sprite >().each(
    //   [&](auto entity, auto& transform, auto& sprite)
    //   {
    //     sprite.sprite.setPosition(transform.x, transform.y);
    //     sprite.sprite.setRotation(transform.radians * 180 / M_PI);
    //     registry.ctx< entt::dispatcher >()
    //       .enqueue(UI::RenderDrawableEvent{ &sprite.sprite });
    //   }
    // );
    
    // registry.view< Transform, Text >().each(
    //   [&](auto entity, auto& transform, auto& text)
    //   {
    //     text.text.setPosition(transform.x, transform.y);
    //     text.text.setRotation(transform.radians * 180 / M_PI);
    //     registry.ctx< entt::dispatcher >()
    //       .enqueue(UI::RenderDrawableEvent{ &text.text });
    //   }
    // );

    // Render stage
    // window.clear(sf::Color::Black);
    registry.ctx< entt::dispatcher >().update< UI::RenderDrawableEvent >();
    window.display();
  }

}

