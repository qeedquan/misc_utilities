#!/bin/sh

make
for i in $(seq 0 4)
do
	for j in $(seq 1 2)
	do
		./ftdi-test -i $i -p $j
	done
done
