#!/bin/bash
src="show-mesh"
dep="glad.c"
clang `pkg-config --cflags glfw3 glu` -o $src $src.c $dep `pkg-config --libs glfw3 glu` -lm