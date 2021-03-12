#pragma once

struct Color
{
	uint8_t red, green, blue, alpha;
};

/*
	Renderer provides an interface to draw made available to mruby
*/

struct SfmlRenderer2D
{
	struct RenderOptions
	{
		sf::Vector2f origin, scale;
		sf::Color fill_color, outline_color;
		float outline_thickness = 0.0;

		RenderOptions()
		: scale(1,1)
		{
		}
	};

	sf::RenderWindow& target;
	RenderOptions render_options;

	SfmlRenderer2D(sf::RenderWindow& window)
	: target(window)
	{
	}

	~SfmlRenderer2D()
	{
	}

	static inline sf::Color sfml_color(const Color& c)
	{
		return sf::Color(c.red, c.green, c.blue, c.alpha);
	}

	// static inline sf::Vector2f sfml_point(const Point2D& p)
	// {
	// 	return sf::Vector2f(p.x, p.y);
	// }

	template< typename Callable >
	inline void with_options(const RenderOptions& new_options, const Callable& function)
	{
		RenderOptions old_options = render_options;
		render_options = new_options;
		function(*this);
		render_options = old_options;
	}

	void text(Transform transform, const std::string& string, float character_size)
	{
		sf::Text text;
		text.setPosition(transform.x, transform.y);
		text.setRotation(transform.radians);
		text.setString(string);
		text.setCharacterSize(character_size);
		text.setScale(render_options.scale);
		text.setOrigin(render_options.origin);
		text.setFillColor(render_options.fill_color);
		text.setOutlineColor(render_options.outline_color);
		text.setOutlineThickness(render_options.outline_thickness);
		target.draw(text);
	}

	void circle(Transform transform, float radius)
	{
		sf::CircleShape shape;
		shape.setPosition(transform.x, transform.y);
		shape.setRadius(radius);
		shape.setRotation(transform.radians);
		shape.setScale(render_options.scale);
		shape.setOrigin(render_options.origin);
		shape.setFillColor(render_options.fill_color);
		shape.setOutlineColor(render_options.outline_color);
		shape.setOutlineThickness(render_options.outline_thickness);
		target.draw(shape);
	}

	void rectangle(Transform transform, float width, float height)
	{
		sf::RectangleShape shape;
		shape.setPosition(transform.x, transform.y);
		shape.setSize(sf::Vector2f(width, height));
		shape.setRotation(transform.radians);
		shape.setScale(render_options.scale);
		shape.setOrigin(render_options.origin);
		shape.setFillColor(render_options.fill_color);
		shape.setOutlineColor(render_options.outline_color);
		shape.setOutlineThickness(render_options.outline_thickness);
		target.draw(shape);
	}

	void polygon(Transform transform, unsigned pointc, sf::Vector2f* pointv)
	{
		sf::ConvexShape shape(pointc);
		for(unsigned i = 0; i < pointc; ++i)
			shape.setPoint(i, pointv[i]);
		shape.setPosition(transform.x, transform.y);
		shape.setRotation(transform.radians);
		shape.setScale(render_options.scale);
		shape.setOrigin(render_options.origin);
		shape.setFillColor(render_options.fill_color);
		shape.setOutlineColor(render_options.outline_color);
		shape.setOutlineThickness(render_options.outline_thickness);
		target.draw(shape);
	}

	void sprite(Transform transform, const std::string& filename)
	{
		sf::Sprite sprite;
		sprite.setPosition(transform.x, transform.y);
		sprite.setRotation(transform.radians);
		sprite.setScale(render_options.scale);
		sprite.setOrigin(render_options.origin);
		target.draw(sprite);
	}

};
