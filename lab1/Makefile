default:lab1b.c
	gcc -g -std=gnu99 -Wall -o simpsh lab1b.c

check:default
	./test.sh

clean:  
	rm -rf lab1-304576879.tar.gz simpsh *.txt *.txt# *~

dist:
	tar -czvf lab1-304576879.tar.gz lab1b.c Makefile README test.sh
