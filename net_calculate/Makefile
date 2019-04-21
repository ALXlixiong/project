.PHONY:all
all:server client
.PHONY:server 
server:server.cc
	g++ $^ -g -o $@ -lpthread -std=c++11 
.PHONY:client
client:client.cc 
	g++ $^ -g -o $@ -lpthread -std=c++11

.PHONY:clean
clean:
	rm server client
