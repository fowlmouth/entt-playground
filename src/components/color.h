
#pragma once

#include <memory>

struct Color
{
  sf::Color color;
};



template<>
struct MRuby::ComponentInterface< Color >
: MRuby::DefaultComponentInterface< Color >
{
  static mrb_value get(mrb_state* state, entt::registry& registry, entt::entity entity, entt::registry::component_type type)
  {
    if(auto shape = registry.try_get< Color >(entity))
    {
      MRuby::HashBuilder hash(state);
      sf::Shape* sf_shape = shape->shape.get();
      if(auto circle = dynamic_cast< sf::CircleShape* >(sf_shape))
        hash("shape", mrb_str_new_cstr(state,"circle"))
          ("radius", mrb_float_value(state, circle->getRadius()))
        ;
      return hash.self;
    }
    return mrb_nil_value();
  }

  static mrb_value set(mrb_state* state, entt::registry& registry, entt::entity entity, entt::registry::component_type type, mrb_int argc, mrb_value* arg)
  {
    if(!argc || ! mrb_hash_p(arg[0]))
      return mrb_nil_value();

    MRuby::HashReader hash(state, arg[0]);
    std::string shape_c;
    hash("shape", shape_c);
    if(shape_c.empty())
      return mrb_nil_value();

    if(shape_c == "circle")
    {
      float radius = 10.0;
      hash("radius", radius);

      registry.assign_or_replace< Shape >(entity, std::make_unique< sf::CircleShape >(radius));
      return hash.self;
    }

    return mrb_nil_value();

  }
};

