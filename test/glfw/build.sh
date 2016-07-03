#!/bin/bash
src="app"
dep="glad.c main.c gui.c"
clang \
`pkg-config --cflags glfw3 glu` \
-o $src $src.c $dep \
`pkg-config --libs glfw3 glu` -lm && \
./$src russia.mesh