#!/bin/sh

set -xe

CFLAGS="-Wall -Wextra `pkg-config --cflags raylib`"
LIBS="`pkg-config --libs raylib`"
source .env

clang $CFLAGS -fPIC -shared -o build/libplug.dylib src/plug.c $LIBS

clang $CFLAGS -o build/musicui src/musicui.c $LIBS
