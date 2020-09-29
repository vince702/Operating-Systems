rm lab2b_list.csv
rm lab2b_list1.csv
rm lab2b_list2.csv
rm lab2b_list3.csv
rm lab2b_list4.csv
rm lab2b_list5.csv

threads=(1, 2, 4, 8, 12, 16, 24)
iterations=(1000)

for i in "${threads[@]}"
do
    for k in "${iterations[@]}"
    do
	./lab2_list --iterations=$k --threads=$i --sync=m >> lab2b_list1.csv
	done
done

for i in "${threads[@]}"
do
    for k in "${iterations[@]}"
    do
	./lab2_list --iterations=$k --threads=$i --sync=s >> lab2b_list1.csv
	done
done

threads=(1, 2, 4, 8, 16, 24)
iterations=(1000)
for i in "${threads[@]}"
do
    for k in "${iterations[@]}"
    do
	./lab2_list --iterations=$k --threads=$i --sync=m >> lab2b_list2.csv
	done
done

threads=(1, 4, 8, 12, 16)
iterations=(1, 2, 4, 8, 16)

for i in "${threads[@]}"
do
    for k in "${iterations[@]}"
    do
	./lab2_list --iterations=$k --threads=$i --yield=id --lists=4 >> lab2b_list3.csv
	done
done

threads=(1, 4, 8, 12, 16)
iterations=(10, 20, 40, 80)

for i in "${threads[@]}"
do
    for k in "${iterations[@]}"
    do
	./lab2_list --iterations=$k --threads=$i --sync=m --yield=id --lists=4 >> lab2b_list3.csv
	done
done

for i in "${threads[@]}"
do
    for k in "${iterations[@]}"
    do
	./lab2_list --iterations=$k --threads=$i --sync=s --yield=id --lists=4 >> lab2b_list3.csv
	done
done


iterations=(1000)
lists=(1, 4, 8, 16)
threads=(1, 2, 4, 8, 12)

for i in "${threads[@]}"
do
    for k in "${iterations[@]}"
    do
    	for l in "${lists[@]}"
    	do
			./lab2_list --iterations=$k --threads=$i --sync=m --lists=$l >> lab2b_list4.csv
		done
	done
done


for i in "${threads[@]}"
do
    for k in "${iterations[@]}"
    do
    	for l in "${lists[@]}"
    	do
			./lab2_list --iterations=$k --threads=$i --sync=s --lists=$l >> lab2b_list5.csv
		done
	done
done

cat lab2b_list1.csv >> lab2b_list.csv
cat lab2b_list2.csv >> lab2b_list.csv
cat lab2b_list3.csv >> lab2b_list.csv
cat lab2b_list4.csv >> lab2b_list.csv
cat lab2b_list5.csv >> lab2b_list.csv
