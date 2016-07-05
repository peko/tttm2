#!/bin/bash
dep="main.c glad.c gui.c types.c"
clang                                      \
`pkg-config --cflags glfw3 glu`            \
-o mesher $dep                             \
`pkg-config --libs glfw3 glu` -lshp -lm && \
./mesher