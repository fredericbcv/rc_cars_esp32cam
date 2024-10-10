#!/usr/bin/sh

docker run -it -v ./../:/project --privileged -v /dev/ttyUSB0:/dev/ttyUSB0  rctools
