
#include "Server.hpp"
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>


Server::~Server()
{
}

void	Server::setupSockets()
{
	const std::vector<t_server> server = _config.getServers();
	size_t index = 0;
	for (std::vector<t_server>::const_iterator it = server.begin(); it != server.end(); it++)
	{
		int	sockfd = socket(AF_INET, SOCK_STREAM, 0);
		if (sockfd == -1)
			throw std::runtime_error("Failed to create socket");
		fcntl(sockfd, F_SETFL, O_NONBLOCK);	
		struct sockaddr_in serverAddr;
		serverAddr.sin_family = AF_INET;
		serverAddr.sin_addr.s_addr = inet_addr(it->host.c_str());
		serverAddr.sin_port = htons(it->port);
		if (bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1)
		{
			close(sockfd);
			throw std::runtime_error("Failed to bind socket");
		}
		if (listen(sockfd, SOMAXCONN) == -1)
		{
			close(sockfd);
			throw std::runtime_error("Failed to listen on socket");
		}
		_socket[index++] = sockfd;
	}
}

void	Server::initEpoll()
{
	_epollFd = epoll_create1(0);
	if (_epollFd == -1)
		throw std::runtime_error("Failed to create epoll instance");
	for (int sockfd : _socket)
	{
		struct epoll_event epoll_ev;
		epoll_ev.events = EPOLLIN;
		epoll_ev.data.fd = sockfd;
		if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, sockfd, &epoll_ev) == -1)
			throw std::runtime_error("Failed to add server socket to epoll");
	}
}

void	Server::handleNewConnection(int socket)
{
	struct sockaddr_in	clientAddr;
	socklen_t			clientAddrlen = sizeof(clientAddr);
	struct	epoll_event epoll_ev;

	int	clientSocket = accept(socket, (struct sockaddr*)&clientAddr, &clientAddrlen);	
	if (clientSocket == -1)
		return; //handle error here
	fcntl(clientSocket, F_SETFL, O_NONBLOCK);
	
	epoll_ev.events = EPOLLIN | EPOLLET;
	epoll_ev.data.fd = clientSocket;
	if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, clientSocket, &epoll_ev) == -1)
	{
		close(clientSocket); // handle error here
		return;
	}
}

void	Server::handleClientEvent(int clientSocket, uint32_t event)
{
	if (event & EPOLLIN)
	{
		//handle incoming HTTP request data
		//Read from ClientSocket, parse request..
	}
	if (event & EPOLLOUT)
	{
		//handle outgoing data (HTTP request)
		//Read from clientSocket, parse HTTP request
	}
	if (event & (EPOLLRDHUP | EPOLLHUP))
	{
		//client disconnected
		epoll_ctl(_epollFd, EPOLL_CTL_DEL, clientSocket, NULL);
		close(clientSocket);
	}
}

void	Server::eventLoop()
{
	struct epoll_event	events[MAX_EVENTS];
	while (true)
	{
		int nfds = epoll_wait(_epollFd, events, MAX_EVENTS, -1);
		if (nfds == -1)
			throw std::runtime_error("epoll_wait failed");
		for (int i = 0; i < nfds; i++)
		{
			if (std::find(_socket.begin(), _socket.end(), events[i].data.fd) != _socket.end()) // rethink the iteration over the sockets, find a way to search for the sockets directly?
				handleNewConnection(events[i].data.fd);
			else
				handleClientEvent(events[i].data.fd, events[i].events); // filter for cgis and handle them differently
		}
	}
}

void	Server::start()
{
	setupSockets();
	initEpoll();
	eventLoop();
}
