#!/bin/bash

rm lab2_add.csv

threads=(1, 2, 4, 8, 12)
iterations=(100, 1000, 10000, 100000)

for i in "${threads[@]}"
do
    for k in "${iterations[@]}"
    do
	./lab2_add --iterations=$k --threads=$i >> lab2_add.csv
	done
done

threads=(2, 4, 8, 12)
iterations=(10, 20, 40, 80, 100, 1000, 10000, 100000)

for i in "${threads[@]}"
do
    for k in "${iterations[@]}"
    do
	./lab2_add --iterations=$k --threads=$i --yield >> lab2_add.csv
	done
done


threads=(2, 4, 8, 12)
iterations=(10000)

for i in "${threads[@]}"
do
    for k in "${iterations[@]}"
    do
	./lab2_add --iterations=$k --threads=$i --yield --sync=m >> lab2_add.csv
	done
done

threads=(2, 4, 8, 12)
iterations=(10000)

for i in "${threads[@]}"
do
    for k in "${iterations[@]}"
    do
	./lab2_add --iterations=$k --threads=$i --yield --sync=c >> lab2_add.csv
	done
done

threads=(2, 4, 8, 12)
iterations=(1000)

for i in "${threads[@]}"
do
    for k in "${iterations[@]}"
    do
	./lab2_add --iterations=$k --threads=$i --yield --sync=s >> lab2_add.csv
	done
done


threads=(1, 2, 4, 8, 12)
iterations=(10000)

for i in "${threads[@]}"
do
    for k in "${iterations[@]}"
    do
	./lab2_add --iterations=$k --threads=$i  >> lab2_add.csv
	done
done



threads=(1, 2, 4, 8, 12)
iterations=(10000)

for i in "${threads[@]}"
do
    for k in "${iterations[@]}"
    do
	./lab2_add --iterations=$k --threads=$i --sync=m  >> lab2_add.csv
	done
done





threads=(1, 2, 4, 8, 12)
iterations=(10000)

for i in "${threads[@]}"
do
    for k in "${iterations[@]}"
    do
	./lab2_add --iterations=$k --threads=$i --sync=s >> lab2_add.csv
	done
done





threads=(1, 2, 4, 8, 12)
iterations=(10000)

for i in "${threads[@]}"
do
    for k in "${iterations[@]}"
    do
	./lab2_add --iterations=$k --threads=$i --sync=c  >> lab2_add.csv
	done
done
