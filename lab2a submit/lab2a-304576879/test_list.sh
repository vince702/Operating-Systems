#!/bin/bash

rm lab2_list.csv

threads=(1)
iterations=(10, 100, 1000, 10000, 20000)

for i in "${threads[@]}"
do
    for k in "${iterations[@]}"
    do
	./lab2_list --iterations=$k --threads=$i >> lab2_list.csv
	done
done


threads=(2, 4, 8, 12)
iterations=(1, 10,100,1000)

for i in "${threads[@]}"
do
    for k in "${iterations[@]}"
    do
	./lab2_list --iterations=$k --threads=$i >> lab2_list.csv
	done
done

##################################conflicts yield ##################################

threads=(2,4,8,12) 
iterations=(1, 2, 4, 8, 16, 32)

for i in "${threads[@]}"
do
    for k in "${iterations[@]}"
    do
	./lab2_list --iterations=$k --threads=$i --yield=i >> lab2_list.csv
	done
done

threads=(2,4,8,12) 
iterations=(1, 2, 4, 8, 16, 32)

for i in "${threads[@]}"
do
    for k in "${iterations[@]}"
    do
	./lab2_list --iterations=$k --threads=$i --yield=d >> lab2_list.csv
	done
done

threads=(2,4,8,12) 
iterations=(1, 2, 4, 8, 16, 32)

for i in "${threads[@]}"
do
    for k in "${iterations[@]}"
    do
	./lab2_list --iterations=$k --threads=$i --yield=il >> lab2_list.csv
	done
done

threads=(2,4,8,12) 
iterations=(1, 2, 4, 8, 16, 32)

for i in "${threads[@]}"
do
    for k in "${iterations[@]}"
    do
	./lab2_list --iterations=$k --threads=$i --yield=dl >> lab2_list.csv
	done
done






####################################################################
threads=(12) 
iterations=( 32)


for i in "${threads[@]}"
do
    for k in "${iterations[@]}"
    do
	./lab2_list --iterations=$k --threads=$i --yield=i --sync=m >> lab2_list.csv
	done
done



for i in "${threads[@]}"
do
    for k in "${iterations[@]}"
    do
	./lab2_list --iterations=$k --threads=$i --yield=d --sync=m >> lab2_list.csv
	done
done


for i in "${threads[@]}"
do
    for k in "${iterations[@]}"
    do
	./lab2_list --iterations=$k --threads=$i --yield=il --sync=m >> lab2_list.csv
	done
done



for i in "${threads[@]}"
do
    for k in "${iterations[@]}"
    do
	./lab2_list --iterations=$k --threads=$i --yield=dl --sync=m >> lab2_list.csv
	done
done


####################################################################

threads=(12) 
iterations=(32)


for i in "${threads[@]}"
do
    for k in "${iterations[@]}"
    do
	./lab2_list --iterations=$k --threads=$i --yield=i --sync=s >> lab2_list.csv
	done
done



for i in "${threads[@]}"
do
    for k in "${iterations[@]}"
    do
	./lab2_list --iterations=$k --threads=$i --yield=d --sync=s >> lab2_list.csv
	done
done


for i in "${threads[@]}"
do
    for k in "${iterations[@]}"
    do
	./lab2_list --iterations=$k --threads=$i --yield=il --sync=s >> lab2_list.csv
	done
done



for i in "${threads[@]}"
do
    for k in "${iterations[@]}"
    do
	./lab2_list --iterations=$k --threads=$i --yield=dl --sync=s >> lab2_list.csv
	done
done

####################################################################



threads=(1, 2, 4, 8, 12, 16, 24)
iterations=(1000)

for i in "${threads[@]}"
do
    for k in "${iterations[@]}"
    do
	./lab2_list --iterations=$k --threads=$i --sync=m >> lab2_list.csv
	done
done



for i in "${threads[@]}"
do
    for k in "${iterations[@]}"
    do
	./lab2_list --iterations=$k --threads=$i --sync=s >> lab2_list.csv
	done
done
