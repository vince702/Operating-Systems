#//Vincent Chi 304576879 vincentchi9702@gmail.com   
all:
	@gcc  -g -Wall -Wextra -std=gnu99  -o lab2_list lab2_list.c SortedList.c  -pthread
	@gcc  -g -Wall -Wextra -std=gnu99  -o lab2_add lab2_add.c  -pthread
build: default 
default:
	@gcc  -g -Wall -Wextra -std=gnu99  -o lab2_list lab2_list.c SortedList.c  -pthread
	@gcc  -g -Wall -Wextra -std=gnu99  -o lab2_add lab2_add.c  -pthread
tests:  default
	@chmod +x test_add.sh
	@chmod +x test_list.sh
	@-./test_add.sh
	@-./test_list.sh

graphs: | lab2_add.csv lab2_list.csv
	@chmod +x lab2_add.gp
	@chmod +x lab2_list.gp
	@gnuplot lab2_add.gp
	@gnuplot lab2_list.gp

lab2_add.csv: tests
lab2_list.csv: tests


clean:
	@rm -f  lab2_add lab2_list lab2a-304576879.tar.gz  


dist: graphs
	tar -czvf  lab2a-304576879.tar.gz lab2_add.gp lab2_list.gp lab2_add.c lab2_list.c SortedList.h SortedList.c Makefile lab2_add.csv lab2_list.csv lab2_add-1.png lab2_add-2.png lab2_add-3.png lab2_add-4.png lab2_add-5.png lab2_list-1.png lab2_list-2.png lab2_list-3.png lab2_list-4.png README test_add.sh test_list.sh




