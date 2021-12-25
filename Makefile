all :
	gcc -Wall -o server/server server/server.c -lpthread
	gcc -Wall -o client/client client/client.c