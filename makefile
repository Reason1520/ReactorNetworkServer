all: server client

server: server.cpp InetAddress.cpp
	g++ -o server server.cpp InetAddress.cpp

client: client.cpp
	g++ -o client client.cpp

clean:
	rm -f server client