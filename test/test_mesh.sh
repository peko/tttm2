src="../src"
dep="../src/mesh.c ../src/shape.c ../src/point.c"
inc="-I../src"
lib="-lshp -lproj -lm"
tst=test_mesh
clang $tst.c $dep $inc -o $tst $lib && 
./test_mesh

# gnuplot> plot '/tmp/test_mesh.mesh' using 1:2 with lines

# valgrind --leak-check=full ./test_mesh ../data/earth_110m.shp
