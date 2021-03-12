#pragma once

struct Text
{
  sf::Text text;
};

MRUBY_COMPONENT_INTERFACE_BEGIN(Text)

  MRUBY_COMPONENT_GET
  {
    if(auto text = registry.try_get< Text >(entity))
    {
      auto font = text->text.getFont();
      auto& font_cache = registry.ctx< UI::FontCache >();
      mrb_value font_name = mrb_nil_value();
      for(const auto& fn : font_cache.cache)
      {
        if(fn.second.asset.get() == font)
        {
          font_name = mrb_str_new_cstr(state, fn.first.c_str());
          break;
        }
      }

      auto string = text->text.getString().toAnsiString();

      mrb_value results[3]{
        mrb_str_new_cstr(state, string.c_str()),
        mrb_fixnum_value(text->text.getCharacterSize()),
        font_name // mrb_str_new_cstr(state, font_cache.get_asset) mrb_nil_value()
      };

      return mrb_ary_new_from_values(state, 3, results);
    }
    return mrb_nil_value();
  }

  MRUBY_COMPONENT_SET
  {
    if(argc != 3)
      return mrb_nil_value();

    auto& text = registry.get_or_emplace< Text >(entity); //, std::make_unique< sf::Text >());
    if(mrb_string_p(argv[0]))
    {
      const char* cstr = mrb_string_value_cstr(state, argv);
      text.text.setString(cstr);
    }
    if(mrb_fixnum_p(argv[1]))
      text.text.setCharacterSize(mrb_int(state, argv[1]));
    if(mrb_string_p(argv[2]))
    {
      auto font = registry.ctx< UI::FontCache >().get(mrb_string_value_cstr(state, &argv[2]));
      if(font)
        text.text.setFont(*font);
    }

    return *argv;
  }

MRUBY_COMPONENT_INTERFACE_END
