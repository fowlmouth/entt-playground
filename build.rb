#!/usr/bin/env ruby

require'optparse'

opts = {
  entt: 'libs/entt',
  cc: 'clang++',
  output: 'game',
  cfiles: 'src/game.cc',

  entt_mruby: 'libs/entt-mruby',
  entt_sfml: 'libs/entt-sfml',

  I: [],
  L: [],

  l: ['mruby']
}

opts[:entt_box2d] = 'libs/entt-box2d'
opts[:I] << 'libs/Box2D/include'
opts[:L] << 'libs/Box2D/build/src'
opts[:l] << 'box2d'

OptionParser.new do |o|
  o.on '--entt=VALUE', 'path to entt directory' do |entt|
    opts[:entt] = entt
  end

  o.on '--entt-mruby=dir', 'path to entt-mruby project dir' do |mr|
    opts[:entt_mruby] = mr
  end

  o.on '--cc=COMPILER', 'c++ compiler' do |cc|
    opts[:cc] = cc
  end

  o.on '--output=BINARYNAME', 'name of binary output' do |file|
    opts[:output] = file
  end

  o.on '--cfiles=FILES', 'c++ source files (space separated)' do |cfiles|
    opts[:cfiles] = cfiles
  end

  o.on '-I=DIR', 'c++ header file dir' do |dir|
    (opts[:I] ||= []) << dir
  end

  o.on '-l=lib', 'c link library' do |lib|
    (opts[:l] ||= []) << lib
  end

  o.on '-L=dir', 'c library search path' do |lib|
    (opts[:L] ||= []) << lib
  end

end.parse!

fail = false
opts.each {|k,v|
  if v.nil?
    fail = true
    puts "Missing option: #{k}"
  end
}
abort if fail

cmd = "#{opts[:cc]} \
  -g -std=c++1z \
  #{`pkg-config --cflags sfml-graphics`.strip} \
  #{"-I #{opts[:entt_mruby]}/include" if opts[:entt_mruby]} \
  #{"-I #{opts[:entt_sfml]}/include" if opts[:entt_sfml]} \
  #{"-I #{opts[:entt_box2d]}/include" if opts[:entt_box2d]} \
  -I #{opts[:entt]}/src \
  #{opts[:I].map{|i|"-I#{i}"}.join(' ') if opts[:I] && !opts[:I].empty?} \
  #{opts[:cfiles]} \
  -o #{opts[:output]} \
  #{`pkg-config --libs sfml-graphics`.strip} \
  #{opts[:L].map{|dir|"-L#{dir}"}.join(' ') if opts[:L]} \
  #{opts[:l].map{|l|"-l#{l}"}.join(' ') if opts[:l]}"

puts "Running command: #{cmd}"
exit Kernel.system cmd
