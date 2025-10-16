all: server client

SERVERFILES = server.cpp InetAddress.cpp Socket.cpp Epoll.cpp Channel.cpp EventLoop.cpp TCPServer.cpp Acceptor.cpp Connection.cpp

server: $(SERVERFILES)
	g++ -o server $(SERVERFILES)

client: client.cpp
	g++ -o client client.cpp

clean:
	rm -f server client