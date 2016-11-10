obj = genlist.o taskqueue.o pthreadpool.o serroutine.o databasectl.o server.o

all:../output/server_app ../output/client_app

../output/server_app:$(obj)
	gcc $^ -o $@  -lpthread -lsqlite3


../output/client_app:client.o user_func.o
	gcc $^ -o $@ 
