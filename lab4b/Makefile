#NAME: VINCENT CHI
#ID: 304576879
#EMAIL: vincentchi9702@gmail.com
default:
	gcc -lm -lmraa -Wall -Wextra -o lab4b lab4b.c

check: default
	chmod +x check.sh
	./check.sh

clean:
	rm -f lab4b-304576879.tar.gz lab4b

dist:
	tar -czvf lab4b-304576879.tar.gz lab4b.c check.sh  Makefile README
