server : server.o client.o
	gcc -o server server.o
	gcc -o client client.o

main.o : main.c
	gcc -c main.c

client.o : client.c
	gcc -c client.c
clean : 
	rm -f *.o