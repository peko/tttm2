src="../src"

dep="$src/primitives.c"

lib="-lm"
inc="-I$src"
tst="test_primitives"
echo $inc
clang $tst.c $dep $inc -g -o $tst $lib && ./$tst

# gnuplot > plot '/tmp/test_shape.polygons' using 1:2 with lines

# valgrind --leak-check=full ./test_shape ../data/earth_110m.shp
