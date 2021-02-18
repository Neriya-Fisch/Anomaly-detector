
#include "Server.h"

// the "technical part" of the server. sets up the socket connection
Server::Server(int port) throw(const char *)
{
	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd < 0)
		throw "socket failed";
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(port);
	if (bind(fd, (struct sockaddr *)&server, sizeof(server)) < 0)
		throw "bind failed";
	if (listen(fd, 3) < 0)
		throw "listen failed";
}

//runs a thread which connect to clients, one by one.
void Server::start(ClientHandler &ch) throw(const char *)
{
	t = new thread([&ch, this]() {
		while (!stopThread)
		{
			socklen_t clientSize = sizeof(client);
			int aClient = accept(fd, (struct sockaddr *)&client, &clientSize);
			if (aClient < 0)
				throw "accept failed";
			ch.handle(aClient);
			close(aClient);
		}
		close(fd);
	});
}

// stop the connection and finishes the thread.
void Server::stop()
{
	stopThread = true;
	t->join();
}

Server::~Server()
{
}
