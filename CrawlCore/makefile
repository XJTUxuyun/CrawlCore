CC = gcc
object = cJSON.o hashmap.o	list.o	uuid4.o	tools.o	 server.h server.c	assistant.h assistant.c	server_tcp.h	server_tcp.c	server_udp.h	server_udp.c	db_backend.h	db_backend.c	process_request.h	process_request.c	version.h main.c
crawl-core : $(object)  
	$(CC)  $(object)  -lsqlite3 -luv -L /usr/local/Cellar/libuv/HEAD-69c43d9/lib -llog4c -L /usr/local/Cellar/log4c/1.2.4/lib -I /usr/local/Cellar/libuv/HEAD-69c43d9/include/
cJSON.o	: cJSON.h cJSON.c
	$(CC) -c cJSON.h cJSON.c
hashmap.o : hashmap.h hashmap.c
	$(CC) -c hashmap.h hashmap.c
list.o : list.h list.c
	$(CC) -c list.h list.c
uuid4.o : uuid4.h uuid4.c
	$(CC)	-c uuid4.h uuid4.c
tools.o	: tools.h tools.c
	$(CC) -c tools.h tools.c -luv 
config.o : config.h	config.c
	$(CC) -c config.h config.c -lxml2 -I /usr/include/libxml2 -L /usr/lib/
clean:
	rm *.o *.gch