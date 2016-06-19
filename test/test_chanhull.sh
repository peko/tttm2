src="../src"

dep="$src/shape.c $src/point.c"
chp="$src/chanhull"
chl="$chp/chanhull.c $chp/heaphull.c $chp/throwaway.c"

lib="-lshp -lproj -lm"
inc="-I$src -I$chp"
tst="test_chanhull"
echo $inc
clang $tst.c $dep $chl $inc -o $tst $lib && ./$tst

# gnuplot > plot '/tmp/test_shape.polygons' using 1:2 with lines

# valgrind --leak-check=full ./test_shape ../data/earth_110m.shp
