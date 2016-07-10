#!/bin/bash

dep="main.c glad.c gui.c types.c wire.c vbo.c shape.c"

clang                                             \
`pkg-config --cflags glfw3 glu`                   \
-o mesher -g -Wall                                \
$dep                                              \
`pkg-config --libs glfw3 glu` -lshp -lproj -lm && \
./mesher