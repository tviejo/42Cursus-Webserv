
#pragma once
#include "webserv.hpp"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>

#define MAX_SOCKETS 1024
#define MAX_EVENTS 10
#define IO_BUFFER_SIZE 8192

struct SockInfos
{
	t_server	*server;
	bool		isServer;
};

class Server
{
	private:
		const Config					_config;
		int								_epollFd;
		std::map<int, SockInfos>		_sockets;   // map for all sockets (servers listened and connected to client)
		std::map<int, std::string>		_partialRequest; // TODO: replace _partialRequest with _requestStreams 
														 // for efficiency and mandatory to handle binary stuff (POST bin file)
		//std::map<int, std::ofstream>	_requestStreams;
		
		std::map<int, class OutgoingData *>	_responses;
	
		Server();
	public:
		Server(Config &config) : _config(config) {};
		~Server() {};
		void	init();
		void	run();
		void	setupListeningSockets();
		void	eventLoop();
		void	handleNewConnection(int socket);
		void	handleClientEvent(int socket, uint32_t event);
		void	processRequest(int clientSocket, const std::string& request);
		void	sendResponse(int clientSocket, OutgoingData *response);
		void	handleOutgoingData(int clientSocket);
		ssize_t	safeRecv(int socketfd, void *buffer, size_t len, int flags);
		void	shutDown();
		bool	isAlive();
};
