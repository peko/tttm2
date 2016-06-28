src="../src"

dep="$src/ttree.c $src/shape.c $src/primitives.c"
dep="$dep $src/chanhull.c $src/heaphull.c $src/throwaway.c"

lib="-lshp -lproj -lm"
inc="-I$src"
tst="test_ttree"
echo $inc
clang $tst.c $dep $inc -g -o $tst $lib && ./$tst

# gnuplot > plot '/tmp/test_shape.polygons' using 1:2 with lines

# valgrind --leak-check=full ./test_shape ../data/earth_110m.shp
