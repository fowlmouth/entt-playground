#pragma once

struct Transform
{
  double x = 0, y = 0;
  double radians = 0;
};

MRUBY_COMPONENT_INTERFACE_BEGIN(Transform)

  MRUBY_COMPONENT_GET
  {
    auto& transform = registry.get_or_emplace< Transform >(entity);

    mrb_value results[3]{
      mrb_float_value(state, transform.x),
      mrb_float_value(state, transform.y),
      mrb_float_value(state, transform.radians)
    };

    return mrb_ary_new_from_values(state, 3, results);
  }

  MRUBY_COMPONENT_SET
  {
    if(argc != 3)
      return mrb_nil_value();

    Transform new_transform;
    mrb_value x,y,radians;

    new_transform.x = mrb_to_flo(state, argv[0]);
    new_transform.y = mrb_to_flo(state, argv[1]);
    new_transform.radians = mrb_to_flo(state, argv[2]);

    registry.emplace_or_replace< Transform >(entity, new_transform);

    return *argv;
  }

MRUBY_COMPONENT_INTERFACE_END
