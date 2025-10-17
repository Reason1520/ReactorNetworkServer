all: server client2

SERVERFILES = server.cpp InetAddress.cpp Socket.cpp Epoll.cpp Channel.cpp EventLoop.cpp TCPServer.cpp Acceptor.cpp Connection.cpp Buffer.cpp EchoServer.cpp

server: $(SERVERFILES)
	g++ -o server $(SERVERFILES)

client: client2.cpp
	g++ -o client client2.cpp

clean:
	rm -f server client2