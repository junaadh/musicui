#!/bin/sh

set -xe
release="--release"

CFLAGS="-Wall -Wextra `pkg-config --cflags raylib`"
LIBS="`pkg-config --libs raylib`"
mkdir -p build

clang $CFLAGS -o build/libplug.dylib -shared -fPIC src/plug.c $LIBS
if [ $1 == $release ]; then
  clang $CFLAGS -o build/musicui src/plug.c src/musicui.c $LIBS
else
  clang $CFLAGS -DHOTRELOAD -o build/musicui src/musicui.c $LIBS
fi

source .env
