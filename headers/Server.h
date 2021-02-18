/*
 * Server.h
 *
 *  Created on: Dec 13, 2020
 *      Author: Eli
 */

#ifndef SERVER_H_
#define SERVER_H_
#include "commands.h"
#include "CLI.h"
#include <pthread.h>
#include <thread>
using namespace std;

class ClientHandler
{

public:
	virtual void handle(int clientID) = 0;
};

//runs the communication with the client
class AnomalyDetectionHandler : public ClientHandler
{
public:
	AnomalyDetectionHandler() {}
	virtual void handle(int clientID)
	{
		SocketIO sock(clientID);
		CLI cli(&sock);
		cli.start();
	}
};

class Server
{
	thread *t;
	int fd;
	sockaddr_in server;
	sockaddr_in client;
	volatile bool stopThread = false;

public:
	Server(int port) throw(const char *);
	virtual ~Server();
	void start(ClientHandler &ch) throw(const char *);
	void stop();
};

#endif /* SERVER_H_ */
