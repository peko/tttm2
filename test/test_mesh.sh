src="../src"

dep="$src/mesh.c $src/shape.c $src/point.c"
dep="$dep $src/chanhull.c $src/heaphull.c $src/throwaway.c"

lib="-lshp -lproj -lm"
inc="-I$src"
tst="test_mesh"
echo $inc
clang $tst.c $dep $inc -g -o $tst $lib && ./$tst

# gnuplot > plot '/tmp/test_shape.polygons' using 1:2 with lines

# valgrind --leak-check=full ./test_shape ../data/earth_110m.shp
