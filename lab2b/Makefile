#//Vincent Chi 304576879 vincentchi9702@gmail.com   
all:
	@gcc  -g -lprofiler -Wall -Wextra -std=gnu99  -o lab2_list lab2_list.c SortedList.c  -pthread
build: default 
default:
	@gcc  -g -lprofiler -Wall -Wextra -std=gnu99  -o lab2_list lab2_list.c SortedList.c  -pthread
tests:  default
	@chmod +x test2b.sh
	@-./test2b.sh


graphs: | lab2b_list.csv lab2b_list1.csv lab2b_list2.csv lab2b_list3.csv lab2b_list4.csv lab2b_list5.csv
	@chmod +x lab2_list.gp
	@gnuplot lab2_list.gp


lab2b_list.csv:tests
lab2b_list1.csv:tests
lab2b_list2.csv:tests
lab2b_list3.csv:tests
lab2b_list4.csv:tests
lab2b_list5.csv:tests

profile:
	LD_PRELOAD="/usr/lib64/libprofiler.so" CPUPROFILE=./raw.gperf ./lab2_list --iterations=1000 --threads=12 --sync=s
	pprof --text ./lab2_list ./raw.gperf > profile.out
	pprof --list=function ./lab2_list ./raw.gperf >> profile.out

clean:
	@rm -f   lab2_list lab2b-304576879.tar.gz raw.gperf


dist: graphs profile
	tar -czvf  lab2b-304576879.tar.gz test2b.sh Makefile README SortedList.c SortedList.h lab2_list.c lab2b_1.png lab2b_2.png lab2b_3.png lab2b_4.png lab2b_5.png lab2_list.gp lab2b_list.csv  profile.out lab2b_list1.csv lab2b_list2.csv lab2b_list3.csv lab2b_list4.csv lab2b_list5.csv

