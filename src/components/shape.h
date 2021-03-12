#pragma once
#include <memory>

struct Shape
{
  std::unique_ptr< sf::Shape > shape;
};

MRUBY_COMPONENT_INTERFACE_BEGIN(Shape)

  MRUBY_COMPONENT_GET
  {
    if(auto shape = registry.try_get< Shape >(entity))
    {
      MRuby::HashBuilder builder(state);
      sf::Shape* sf_shape = shape->shape.get();
      if(auto circle = dynamic_cast< sf::CircleShape* >(sf_shape))
      {
        builder
          ("shape", mrb_str_new_cstr(state,"circle"))
          ("radius", mrb_float_value(state, circle->getRadius()));
      }
      else if(auto rect = dynamic_cast< sf::RectangleShape* >(sf_shape))
      {
        builder
          ("shape", "rect")
          ("width", mrb_float_value(state, rect->getSize().x))
          ("height", mrb_float_value(state, rect->getSize().y));
      }
      else
        return mrb_nil_value();

      builder
        ("outline_thickness", sf_shape->getOutlineThickness())
        ("outline_color", sf_shape->getOutlineColor())
        ("fill_color", sf_shape->getFillColor())
        ("origin", sf_shape->getOrigin());

      return builder.self;
    }
    return mrb_nil_value();
  }

  MRUBY_COMPONENT_SET
  {
    if(!argc || ! mrb_hash_p(argv[0]))
      return mrb_nil_value();

    MRuby::HashReader reader(state, argv[0]);
    std::string shape_c;
    reader("shape", shape_c);
    if(shape_c.empty())
      return mrb_nil_value();

    if(shape_c == "circle")
    {
      float radius = 10.0;
      reader("radius", radius);

      registry.emplace_or_replace< Shape >(entity, std::make_unique< sf::CircleShape >(radius));
    }
    else if(shape_c == "rect")
    {
      float w = 1, h = 1;
      reader("width", w)("height", h);

      registry.emplace_or_replace< Shape >(entity, std::make_unique< sf::RectangleShape >(sf::Vector2f(w,h)));
    }
    else
      return mrb_nil_value();

    sf::Shape* shape = registry.get< Shape >(entity).shape.get();

    float fval;
    sf::Color cval;
    sf::Vector2f vval;

    if(reader.read_hash("outline_thickness", fval))
      shape->setOutlineThickness(fval);
    if(reader.read_hash("outline_color", cval))
      shape->setOutlineColor(cval);
    if(reader.read_hash("fill_color", cval))
      shape->setFillColor(cval);
    if(reader.read_hash("origin", vval))
      shape->setOrigin(vval);

    return reader.self;
  }

MRUBY_COMPONENT_INTERFACE_END
