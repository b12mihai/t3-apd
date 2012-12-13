#!/bin/bash

make
exec=tema3
num_procs=$1

echo -e "$exec Testing Mandelbrot: \n"

for i in 1 2 3 4
do
	echo -e "Test $i : "
	file_in="test/mandelbrot${i}.in"
	file_out="mandel${i}.pgm"
	original="test/mandelbrot${i}.pgm"
	mpirun -np $num_procs $exec $file_in $file_out
	test/imgdiff $original $file_out
done

echo -e "Testing Julia: \n"
for i in 1 2 3 4 5 6
do
	echo -e "Test $i : "
	file_in="test/julia${i}.in"
	file_out="jul${i}.pgm"
	original="test/julia${i}.pgm"
	mpirun -np $num_procs $exec $file_in $file_out
	test/imgdiff $original $file_out
done
