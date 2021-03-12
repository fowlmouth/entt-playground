=begin

This file is loaded by the game binary, not run by itself

=end



puts global_variables
puts "----"
puts $registry.methods.sort
p @registry

p e1 = $registry.create
p $registry.valid? e1

$registry.set_controller_callback 'move-up' do |registry, entity|
    entity = registry.entity entity

    x,y,rot = entity.get 'Transform'

    y -= 10

    entity.set 'Transform', x,y,rot

    puts "MOVE UP! #{entity}"
end

$registry.set_controller_callback 'move-down' do |registry, entity|
    entity = registry.entity entity

    x,y,rot = entity.get 'Transform'

    y += 10

    entity.set 'Transform', x,y,rot

    puts "MOVE DOWN! #{entity}"
end

$registry.set_controller_callback 'close-window' do |registry, entity|
    registry.close_window
end

# kb = $registry.build_controller 'kb1', [
#   ['move-up', 'Key:Up'],
#   ['move-down', 'Key:Down'],
#   ['close-window', 'Key:Escape'],
#   ['draw-thing', 'Key:Shift', 'MouseButtonDown:Left' 'MouseClick']
# ]

keyboard_controller = $registry.create_controller 'keyboard-1', 'keyboard', {
    'Up' => 'move-up',
    'Down' => 'move-down',
    'Escape' => 'close-window'
}
p keyboard_controller

p $registry.controllers

puts "--------"

def create_paddle x,y
    paddle = $registry.create_entity
    paddle.set 'Transform', x,y,0.0
    paddle.set 'Paddle', true
    paddle.set 'RectangleShape', 10, 50
    paddle
end

def create_ball x,y, vx=0, vy=1
    ball = $registry.create_entity
    ball.set 'Transform', x,y,0.0
    # ball.set 'Velocity', vx, vy
    ball.set 'PhysicsBody', { type: 'dynamic' }
    ball.set 'PhysicsFixture', {
      shape: 'circle',
      radius: 1.0,
      density: 1.0,
      friction: 0.3
    }
    ball.set 'Ball', true
    ball
end

@registry.world_gravity = [0,6]

@player_paddle = create_paddle 10.0, 10.0
@enemy_paddle = create_paddle 780.0, 10.0
@ball = create_ball 100,100

$registry.take_controller @player_paddle.id, keyboard_controller

@player_score = 0
@enemy_score = 0

@score = $registry.create_entity
@score.set 'Transform', 350.0, 10.0
@score.set 'Text', '0 - 0', 32, '/System/Library/Fonts/Menlo.ttc'

5.times do
    create_ball 100,100, Random::rand*2, Random::rand*2
end

module Colors
  White = [255, 255, 255]
  Black = [0, 0, 0]
  Green = [10, 255, 10]
end

@renderer = $registry.renderer

def update dt
  _, size, font = @score.get 'Text'
  @score.set 'Text', "#{@player_score} - #{@enemy_score}", size, font

  dt = dt.to_f / 1000.0

  @registry.world_step dt

  @registry.each_entity('Transform', 'PhysicsBody') { |entity|
    entity.set('Transform', entity.get('PhysicsBody')[:position])
  }

  @registry.draw_debug_world

  $registry.each_entity('Transform', 'Velocity') {|entity|
    x, y, angle = entity.get 'Transform'
    vx, vy = entity.get 'Velocity'
    x += vx
    y += vy
    velocity_dirty = false
    if x < 0 || x > 800
    	vx *= -1
    	x += vx * 2
        velocity_dirty = true
    end
    if y < 0 || y > 600
        vy *= -1
    	y += vy * 2
        velocity_dirty = true
    end
    entity.set 'Transform', x, y, angle
    entity.set 'Velocity', vx, vy if velocity_dirty
  }

  @registry.each_entity('Ball', 'Transform') do |entity|
    x,y,rot = entity.get 'Transform'
    shape = {
      shape: 'circle',
      x: x,
      y: y,
      fill_color: Colors::White
    }
    @registry.draw shape
  end

  # @registry.each_entity('Paddle', 'Transform') do |entity|
  #   x,y,rot = entity.get 'Transform'
  #   shape = {
  #     shape: 'rect',
  #     x: x,
  #     y: y,
  #     height: 50,
  #     width: 5,
  #     fill_color: Colors::White
  #   }
  #   @registry.draw shape
  # end

  @registry.each_entity 'Transform', 'RectangleShape' do |entity|
    x,y,rot = entity.get 'Transform'
    w,h = entity.get 'RectangleShape'

    @renderer.rectangle x, y, w, h
  end

  text = {
    text: "#{@player_score} - #{@enemy_score}",
    font: '/System/Library/Fonts/Menlo.ttc',
    size: 32,
    fill_color: Colors::White
  }
  @registry.draw text

end

