#!/bin/sh

set -xe

CFLAGS="-Wall -Wextra `pkg-config --cflags raylib`"
LIBS="`pkg-config --libs raylib`"
source .env

clang $CFLAGS -o build/libplug.dylib -shared -fPIC src/plug.c $LIBS

clang $CFLAGS -o build/musicui src/musicui.c $LIBS
