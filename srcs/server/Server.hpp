
#pragma once
#include "webserv.hpp"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>

#define MAX_SOCKETS 1024
#define MAX_EVENTS 10

class Server
{
	private:
		Server() = delete;
		const Config			_config;
		Array<int, MAX_SOCKETS>	_socket;
		int						_epollFd;
	public:
		Server(Config &config) : _config(config) {};
		~Server();
		void	start();
		void	setupSockets();
		void	initEpoll();
		void	eventLoop();
		void	handleNewConnection(int socket);
};


