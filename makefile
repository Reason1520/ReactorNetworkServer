all: server client

server: server.cpp InetAddress.cpp Socket.cpp Epoll.cpp Channel.cpp
	g++ -o server server.cpp InetAddress.cpp Socket.cpp Epoll.cpp Channel.cpp

client: client.cpp
	g++ -o client client.cpp

clean:
	rm -f server client