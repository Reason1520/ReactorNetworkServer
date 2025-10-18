all: server client client2

SERVERFILES = server.cpp InetAddress.cpp Socket.cpp Epoll.cpp Channel.cpp EventLoop.cpp TCPServer.cpp Acceptor.cpp Connection.cpp Buffer.cpp EchoServer.cpp ThreadPool.cpp

server: $(SERVERFILES)
	g++ -o server $(SERVERFILES) -lpthread

client: client.cpp
	g++ -o client client.cpp

client2: client2.cpp
	g++ -o client2 client2.cpp

clean:
	rm -f server client client2