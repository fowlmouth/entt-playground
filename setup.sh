#!/bin/sh

pushd libs

	if [ ! -d Box2D ]; then
		git clone https://github.com/erincatto/Box2D
		mkdir Box2D/build
		pushd Box2D/build
			cmake ..
			make
		popd
	fi


	git clone -b tags/v3.6.0 https://github.com/skypjack/entt/

	for repo in entt-mruby entt-box2d entt-sfml; do
		git clone https://github.com/fowlmouth/$repo
	done

	if ! pkg-config --libs sfml-graphics ; then
		echo "WARNING: pkg-config did not find sfml-graphics, build.rb will not work"
	fi

popd

