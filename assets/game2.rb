

@score = $registry.create_entity
@score.set 'Transform', 350.0, 10.0
@score.set 'Text', '0 - 0', 32, '/System/Library/Fonts/Menlo.ttc'

@entity = $registry.create_entity

@entity.set 'Transform', 10.0, 10.0, 0.0
@entity.set 'Text', 'Hello', 48, '/System/Library/Fonts/Menlo.ttc'
@entity.set 'Velocity', {x: 5, y: 4.5}

5.times{
	e = $registry.create_entity
	e.set 'Transform', Random::rand*800, Random::rand*600, 0
	e.set 'Shape', { shape: 'circle', radius: 20+(Random::rand*15) }
	e.set 'Velocity', { x: Random::rand*10-5, y: Random::rand*10-5 }
}

# ship = $registry.create_entity
# ship.set 'Transform', 30,30,0
# p ship.set 'Sprite', {
#     texture: 'kenney_spaceShooterExtension/PNG/Sprites X2/Ships/spaceShips_001.png'
# }
# p ship.get 'Sprite'

class System
    def initialize *components, &block
        @components = components
        @block = block
    end

    def update registry, dt
        registry.each
        @block.call registry, dt
    end
end

velocity_system = System.new 'Transform', 'Velocity' do |registry, dt, entity_id|
    x,y,angle = registry.get_component entity_id, 'Transform'
    vx,vy = registry.get_component entity_id, 'Velocity'

    x += vx
    y += vy

    if x < 0 || x > 800
        vx *= -1
        x += vx * 2
    end
    if y < 0 || y > 600
        vy *= -1
        y += vy * 2
    end

    registry.set_component entity_id, 'Transform', x,y,angle
    registry.set_component entity_id, 'Velocity', vx,vy
end
