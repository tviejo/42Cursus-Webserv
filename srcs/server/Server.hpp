
#pragma once
#include "webserv.hpp"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>

#define MAX_SOCKETS 1024
#define MAX_EVENTS 10
#define MAX_BUFFER_SIZE 4096

class Server
{
	private:
		Server();
		const Config							_config;
		Array<int, MAX_SOCKETS>					_socket;
		int										_epollFd;
		std::map<int, std::string>	_partialRequest;
	public:
		Server(Config &config) : _config(config) {};
		~Server() {};
		void	init();
		void	run();
		void	setupSockets();
		void	initEpoll();
		void	eventLoop();
		void	handleNewConnection(int socket);
		void	handleClientEvent(int socket, uint32_t event);
		void	processRequest(int clientSocket, const std::string& request);
		void	handleOutgoingData(int clientSocket);
		ssize_t	safeRecv(int socketfd, void *buffer, size_t len, int flags);
};


