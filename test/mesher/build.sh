#!/bin/bash

export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:/usr/local/lib/pkgconfig
export LD_LIBRARY_PATH=LD_LIBRARY_PATH:/usr/local/lib

dep="main.c glad.c gui.c types.c wire.c vbo.c shape.c grid.c text.c"

clang                                             \
`pkg-config --cflags glfw3 glu`                   \
-o mesher -g -Wall                                \
$dep                                              \
`pkg-config --libs glfw3 glu` -lshp -lproj -lm && \
./mesher