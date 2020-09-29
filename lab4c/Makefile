#NAME: Vincent Chi
#EMAIL: vincentchi9702@gmail.com
#ID: 304576879

default:
	gcc -Wall -Wextra -lmraa -lm -g  -o lab4c_tcp lab4c_tcp.c
	gcc -Wall -Wextra -lmraa -lm -lssl -lcrypto -g -o lab4c_tls lab4c_tls.c

clean:
	rm -f lab4c-304576879.tar.gz lab4c_tcp lab4c_tls

dist:
	tar -cvzf lab4c-304576879.tar.gz lab4c_tcp.c lab4c_tls.c Makefile README
