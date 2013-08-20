all: server

server: mapache.o
	gcc mapache.o -o mapache

main.o: mapache.c
	gcc -c mapache.c

#hello.o: hello.c
#	gcc -c hello.c

clean:
	rm -rf *o mapache
