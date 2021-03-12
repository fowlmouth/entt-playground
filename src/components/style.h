#pragma once

struct Style
{
	sf::Color fill_color, outline_color;
	float outline_thickness = 0.0;
};

MRUBY_COMPONENT_INTERFACE_BEGIN(Style)

	MRUBY_COMPONENT_GET
	{
		if(auto style = registry.try_get< Style >(entity))
		{
			MRuby::HashBuilder builder(state);
			builder
				("fill_color", style->fill_color)
				("outline_color", style->outline_color)
				("outline_thickness", style->outline_thickness)
			;
			return builder.self;
		}
		return mrb_nil_value();
	}

	MRUBY_COMPONENT_SET
	{
		if(!argc || !mrb_hash_p(argv[0]))
			return mrb_nil_value();

		Style& style = registry.get_or_emplace< Style >(entity);

		MRuby::HashReader reader(state, argv[0]);
		reader
			("fill_color", style.fill_color)
			("outline_color", style.outline_color)
			("outline_thickness", style.outline_thickness)
		;
		return argv[0];
	}

MRUBY_COMPONENT_INTERFACE_END

