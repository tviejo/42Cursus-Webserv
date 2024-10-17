#include "webserv.hpp"
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>

void	Server::setupSockets()
{
	std::cout << "setupSockets()\n";
	
	_epollFd = epoll_create1(0);
	if (_epollFd == -1)
		throw std::runtime_error("[setupSockets] Failed to create epoll instance");
	struct epoll_event epoll_ev;
	epoll_ev.events = EPOLLIN;

	const std::vector<t_server> &servers = _config.getServers();
	//size_t index = 0;
	for (std::vector<t_server>::const_iterator it = servers.begin(); it != servers.end(); it++)
	{
		int	sock = socket(AF_INET, SOCK_STREAM, 0);
		if (sock == -1)
			throw std::runtime_error("[setupSockets] Failed to create socket");
		fcntl(sock, F_SETFL, O_NONBLOCK);	
		struct sockaddr_in serverAddr;
		serverAddr.sin_family = AF_INET; //This is edge trigger??
		serverAddr.sin_addr.s_addr = inet_addr(it->host.c_str());
		serverAddr.sin_port = htons(it->port);
		if (bind(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1)
		{
			close(sock);
			throw std::runtime_error("[setupSockets] Failed to bind socket");
		}
		if (listen(sock, SOMAXCONN) == -1)
		{
			close(sock);
			throw std::runtime_error("[setupSockets] Failed to listen on socket");
		}
		SockInfos sinfo = {.server = const_cast<t_server *>(&*it), .isServer = true};
		_sockets[sock] = sinfo;
		//_servers[sock] = const_cast<t_server *>(&*it);
		
		epoll_ev.data.fd = sock;
		if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, sock, &epoll_ev) == -1)
			throw std::runtime_error("[setupSockets] Failed to add server socket to epoll");
	}
}

/*void	Server::initEpoll()
{
	std::cout << "initEpoll()\n";

	_epollFd = epoll_create1(0);
	if (_epollFd == -1)
		throw std::runtime_error("Failed to create epoll instance");
	for (const int* it = _socket.begin(); it != _socket.end(); it++)
	{
		struct epoll_event epoll_ev;
		epoll_ev.events = EPOLLIN;
		epoll_ev.data.fd = *it;
		if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, *it, &epoll_ev) == -1)
			throw std::runtime_error("Failed to add server socket to epoll");
	}
}*/

void	Server::init()
{
	std::cout << "init()\n";
	
	setupSockets();
	//initEpoll();
}

void	Server::handleNewConnection(int servSock)
{
	std::cout << "handleNewConnection()\n";
	
	struct sockaddr_in	clientAddr;
	socklen_t			clientAddrlen = sizeof(clientAddr);
	struct	epoll_event epoll_ev;

	int	clientSocket = accept(servSock, (struct sockaddr*)&clientAddr, &clientAddrlen);	
	if (clientSocket == -1)
		throw std::runtime_error("Accept failed");
	fcntl(clientSocket, F_SETFL, O_NONBLOCK);
	epoll_ev.events = EPOLLIN;
	epoll_ev.data.fd = clientSocket;
	if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, clientSocket, &epoll_ev) == -1)
	{
		close(clientSocket);
		throw std::runtime_error("Failed to add to epoll");
	}
	SockInfos sinfos = {.server = _sockets[servSock].server, .isServer = false};
	_sockets[clientSocket] = sinfos;
}

ssize_t	Server::safeRecv(int socketfd, void *buffer, size_t len, int flags)
{
	std::cout << "safeRecv()\n";
	
	ssize_t	result = recv(socketfd, buffer, len, flags);
	if (result == -1)
		throw std::runtime_error("Receive failed");
	return result;	
}

void	Server::handleOutgoingData(int clientSocket)
{
	std::cout << "handleOutgoingData()\n";
	
	std::string &toSend = _partialResponse[clientSocket];
	ssize_t bytes_sent = send(clientSocket, toSend.c_str(), toSend.size(), 0);
	if (bytes_sent < 0)
		throw std::runtime_error("[handleOutgoingData/send] error while sending response to client");
	if ((size_t)bytes_sent < toSend.size())
		_partialResponse[clientSocket].erase(0, bytes_sent);
	else {
		struct epoll_event epoll_ev;
		epoll_ev.events = 0;
		epoll_ev.data.fd = clientSocket;
		if (epoll_ctl(_epollFd, EPOLL_CTL_MOD, clientSocket, &epoll_ev) == -1)
			throw std::runtime_error("[handleOutgoingData/epoll_ctl] failed to modify client socket events to 0");
		shutdown(clientSocket, SHUT_RDWR);
	}
}

void	Server::processRequest(int clientSocket, const std::string& clientRequest)
{
	std::cout << "processRequest()\n";
	
	HTTPRequest	request(clientRequest);
	std::string	response;

	if (request.get_method() == "GET")
		response = handleGetResponse(*_sockets[clientSocket].server, request);
	else if (request.get_method() == "POST")
		response = handlePostResponse(*_sockets[clientSocket].server, request);
	else if (request.get_method() == "DELETE")
		response = handleDeleteResponse(*_sockets[clientSocket].server, request);
	/*else
		response = handleResponse(request);*/
	sendResponse(clientSocket, response);
	
}

void	Server::sendResponse(int clientSocket, std::string &response)
{
	std::cout << "sendResponse()\n";
	
	_partialResponse[clientSocket] = response;
	struct epoll_event epoll_ev;
	epoll_ev.events = EPOLLOUT;
	epoll_ev.data.fd = clientSocket;
	if (epoll_ctl(_epollFd, EPOLL_CTL_MOD, clientSocket, &epoll_ev) == -1)
		throw std::runtime_error("[sendResponse/epoll_ctl] failed to modify client socket events to EPOLLOUT");
}

void	Server::handleClientEvent(int clientSocket, uint32_t event)
{
	std::cout << "handleClientEvent()\n";
	
	if (event & EPOLLIN)
	{
		char		buffer[MAX_BUFFER_SIZE];
	
		try 
		{
			ssize_t	bytesRead = safeRecv(clientSocket, buffer, sizeof(buffer), 0);
			if (bytesRead > 0)
			{
				std::cerr << bytesRead << " bytes received : " << std::string(buffer, bytesRead) << "\n";
				_partialRequest[clientSocket].append(buffer, bytesRead);
				if (_partialRequest[clientSocket].find("\r\n\r\n") != std::string::npos)
				{
					processRequest(clientSocket, _partialRequest[clientSocket]);
					_partialRequest.erase(clientSocket);
				}
			}
			else if (bytesRead == 0)
			{
				std::cerr << "Error ? bytesRead == 0\n";
				epoll_ctl(_epollFd, EPOLL_CTL_DEL, clientSocket, NULL);
				close(clientSocket);
				_partialRequest.erase(clientSocket);
			}
		}
		catch (std::exception &e)
		{
			std::cerr << "Error receiving data from client : " << e.what() << std::endl;
			epoll_ctl(_epollFd, EPOLL_CTL_DEL, clientSocket, NULL);
			close(clientSocket);
			_partialRequest.erase(clientSocket);
		}
	}
	if (event & EPOLLOUT)
	{
		try 
		{
			handleOutgoingData(clientSocket);
		}
		catch (std::exception &e)
		{
			std::cerr << "Error sending data to client : " << e.what() << std::endl;
			epoll_ctl(_epollFd, EPOLL_CTL_DEL, clientSocket, NULL);
			close(clientSocket);
		}
		//handle outgoing data (HTTP request)
		//Read from clientSocket, parse HTTP request
		//Prepare response and sent it to the client

	}
	if (event & (EPOLLRDHUP | EPOLLHUP))
	{
		std::cout << "event: EPOLLRDHUP | EPOLLHUP\n";
		//client disconnected
		epoll_ctl(_epollFd, EPOLL_CTL_DEL, clientSocket, NULL);
		close(clientSocket);
		_partialRequest.erase(clientSocket);
		_sockets.erase(clientSocket);
	}
}

void	Server::run()
{
	std::cout << "Server::run()\n";

	struct epoll_event	events[MAX_EVENTS];
	while (true)
	{
		int nfds = epoll_wait(_epollFd, events, MAX_EVENTS, 100);
		if (nfds == -1)
			throw std::runtime_error("[Server::run()] epoll_wait failed");
		for (int i = 0; i < nfds; i++)
		{
			//if (std::find(_socket.begin(), _socket.end(), events[i].data.fd) != _socket.end()) // rethink the iteration over the sockets, find a way to search for the sockets directly?
			//if (_servers.find(events[i].data.fd) == _servers.end())
			if (_sockets[events[i].data.fd].isServer)
				handleNewConnection(events[i].data.fd);
			else
				handleClientEvent(events[i].data.fd, events[i].events);
		}
		std::cout << "epoll_wait returns; nfds: " << nfds << std::endl;
		if (NonBlockingGetch::getche() == 27)  // Echap to shutdown webserver
		{
			shutDown();
			return;
		}
	}
}

void	Server::shutDown()
{
	std::map<int,SockInfos>::iterator it = _sockets.begin();
	std::map<int,SockInfos>::iterator end = _sockets.end();
	for (; it != end; it++)
	{
		shutdown(it->first, SHUT_RDWR);
		close(it->first);
	}
}
