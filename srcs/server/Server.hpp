
#pragma once
#include "webserv.hpp"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

class Server
{
	private:
		Server() = delete;
		const Config			_config;
		Array<int, MAX_EVENTS>	_socket;
		int						_epollFd;
	public:
		Server(Config &config) : _config(config) {};
		~Server();
		void	start();
		void	setupSockets();
		void	initEpoll();
		void	eventLoop();
};


