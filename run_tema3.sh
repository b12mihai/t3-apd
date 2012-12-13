#!/bin/bash

#Decomment this stuff to run it properly on fep.grid.pub.ro
#. /usr/share/Modules/init/bash
#module load libraries/openmpi-1.6-gcc-4.4.6

EXPECTED_ARGS=1
E_BADARGS=65

if [ $# -ne $EXPECTED_ARGS ]
then
  echo "USAGE: `basename $0` {num_procs}"
  exit $E_BADARGS
fi

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
