#!/bin/sh

if [[ ! -f ./build/mission_file_builder/mission_file_builder ]]; then
	echo "Cannot find program. Make sure you have run colcon build."
	exit # Don't run the script again after we're done
fi

./build/mission_file_builder/mission_file_builder
