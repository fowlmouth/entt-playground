
#pragma once

#include <memory>

struct Sprite
{
  sf::Sprite sprite;
};
  
MRUBY_COMPONENT_INTERFACE_BEGIN(Sprite)

  MRUBY_COMPONENT_GET
  {
    if(auto sprite = registry.try_get< Sprite >(entity))
    {
      MRuby::HashBuilder hash(state);

      auto& sf_sprite = sprite->sprite;
      return mrb_true_value();
    }
    return mrb_nil_value();
  }

  MRUBY_COMPONENT_SET
  {
    if(!argc || ! mrb_hash_p(argv[0]))
      return mrb_nil_value();

    MRuby::HashReader hash(state, argv[0]);
    std::string texture;
    hash("texture", texture);
    if(texture.empty())
      return mrb_nil_value();

    auto& texture_cache = registry.ctx< UI::TextureCache >();
    if(auto asset = texture_cache.get_asset(texture))
    {
      registry.emplace_or_replace< Sprite >(entity, sf::Sprite(*asset->asset)); // std::make_unique< sf::CircleShape >(radius));
      return hash.self;
    }

    return mrb_nil_value();

  }

MRUBY_COMPONENT_INTERFACE_END

