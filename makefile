all: server client

server: server.cpp InetAddress.cpp Socket.cpp Epoll.cpp Channel.cpp EventLoop.cpp TCPServer.cpp
	g++ -o server server.cpp InetAddress.cpp Socket.cpp Epoll.cpp Channel.cpp EventLoop.cpp TCPServer.cpp

client: client.cpp
	g++ -o client client.cpp

clean:
	rm -f server client