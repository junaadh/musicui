#!/bin/sh

set -xe

CFLAGS="-Wall -Wextra `pkg-config --cflags raylib`"
LIBS="`pkg-config --libs raylib`"
RAYLIB_PATH="/opt/homebrew/Cellar/raylib/5.0"

# clang $CFLAGS -o musicui main.c -I$RAYLIB_PATH/include -L$RAYLIB_PATH/lib -lraylib
clang $CFLAGS -o musicui main.c $LIBS
