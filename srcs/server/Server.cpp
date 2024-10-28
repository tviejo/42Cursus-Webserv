#include "webserv.hpp"
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>

void	Server::setupListeningSockets()
{
	std::cout << "setupSockets()\n";
	
	_epollFd = epoll_create1(0);
	if (_epollFd == -1)
		throw std::runtime_error("[setupSockets] Failed to create epoll instance");
	struct epoll_event epoll_ev;
	epoll_ev.events = EPOLLIN;

	const std::vector<t_server> &servers = _config.getServers();
	for (std::vector<t_server>::const_iterator it = servers.begin(); it != servers.end(); it++)
	{
		int	sock = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
		if (sock == -1)
			throw std::runtime_error("[setupSockets] Failed to create socket");
		int	opt = 1;
		int keepalive = 1;

		if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
		{
			close(sock);
			throw std::runtime_error("[setupSockets] Failed to set socket options: reuseaddr");
		}
		if (setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, &keepalive, sizeof(keepalive)) == -1)
		{
			close(sock);
			throw std::runtime_error("[setupSockets] Failed to set socket options: keepalive");
		}
		fcntl(sock, F_SETFL, O_NONBLOCK);	
		struct sockaddr_in serverAddr;
		serverAddr.sin_family = AF_INET;
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
				
		epoll_ev.data.fd = sock;
		if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, sock, &epoll_ev) == -1)
			throw std::runtime_error("[setupSockets] Failed to add server socket to epoll");
	}
}

bool Server::loadDisplayDirectoryHtmlFiles()
{
	std::ifstream ifs("www/directory/directory-part1.html");
	if (!ifs)
		return false;
	std::stringstream ss;
	ss << ifs.rdbuf();
	_displayDirHtmlPart1 = ss.str();
	
	std::ifstream ifs2("www/directory/directory-part2.html");
	if (!ifs2)
		return false;
	std::stringstream ss2;
	ss2 << ifs2.rdbuf();
	_displayDirHtmlPart2 = ss2.str();
	return true;
}

void	Server::init()
{
	std::cout << "init()\n";
	loadDisplayDirectoryHtmlFiles();
	setupListeningSockets();
}

void	Server::handleNewConnection(int servSock)
{
	std::cout << "handleNewConnection(): incoming connection on server port: "
		<< _sockets[servSock].server->port << "  (servSock: " << servSock << ")\n";
	
	struct sockaddr_in	clientAddr;
	socklen_t			clientAddrlen = sizeof(clientAddr);
	struct	epoll_event epoll_ev;

	int	clientSocket = accept(servSock, (struct sockaddr*)&clientAddr, &clientAddrlen);	
	if (clientSocket == -1)
		throw std::runtime_error("[handleNewConnection/accept] Accept failed");
	fcntl(clientSocket, F_SETFL, O_NONBLOCK);
	epoll_ev.events = EPOLLIN;
	epoll_ev.data.fd = clientSocket;
	if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, clientSocket, &epoll_ev) == -1)
	{
		close(clientSocket);
		throw std::runtime_error("[handleNewConnection/epoll_ctl] Failed to add to epoll");
	}
	SockInfos sinfos = {.server = _sockets[servSock].server, .isServer = false};
	_sockets[clientSocket] = sinfos;
	
	char cliIP[16];
	inet_ntop(AF_INET, &clientAddr.sin_addr, cliIP, sizeof(cliIP));
	std::cout << "   -> new client @IP: " << cliIP << ":" << ntohs(clientAddr.sin_port)
		<< " on socket: " << clientSocket << std::endl;
}

ssize_t	Server::safeRecv(int socketfd, void *buffer, size_t len, int flags)
{
	ssize_t	result = recv(socketfd, buffer, len, flags);
	if (result == -1)
		throw std::runtime_error("[safeRecv/recv] Receive failed");
	return result;	
}

void	Server::handleOutgoingData(int clientSocket)
{
	OutgoingData *toSend = _responses[clientSocket];
	ssize_t bytes_sent = send(clientSocket, toSend->getbufptr(), toSend->getbuflen(), MSG_NOSIGNAL);
	if (bytes_sent < 0)
		throw std::runtime_error("[handleOutgoingData/send] error while sending response to client");
	
	//std::cout << std::string(toSend->getbufptr(), toSend->getbuflen()) << "\n";
	if (bytes_sent < toSend->getbuflen())
	{
		toSend->bufferForward(bytes_sent);
	}
	else if (toSend->hasRemainingData())
	{
		toSend->loadBuffer();
	}
	else {
		delete toSend;
		_responses.erase(clientSocket);
		struct epoll_event epoll_ev;
		epoll_ev.events = EPOLLIN;
		epoll_ev.data.fd = clientSocket;
		if (epoll_ctl(_epollFd, EPOLL_CTL_MOD, clientSocket, &epoll_ev) == -1)
			throw std::runtime_error("[handleOutgoingData/epoll_ctl] failed to modify client socket events to 0");
	}
}

void	Server::processRequest(int clientSocket, const std::string& clientRequest)
{
	std::cout << "processRequest()   socket: " << clientSocket << "\n";
	
	HTTPRequest	request(clientRequest);
	std::cout << "  -> " << request << std::endl;
	OutgoingData *response;

	std::cerr << " METHOD: " << request.get_method() << std::endl;
	if (request.get_method() == "GET")
		response = Response::handleGet(*_sockets[clientSocket].server, request, clientSocket, *this);
	else if (request.get_method() == "POST")
		response = Response::handlePost(*_sockets[clientSocket].server, request, clientSocket);
	else if (request.get_method() == "DELETE")
		response = Response::handleDelete(*_sockets[clientSocket].server, request, clientSocket);
	else
		response = Response::makeErrorResponse(405, "Method Not Allowed", *_sockets[clientSocket].server, clientSocket);
	sendResponse(clientSocket, response);
}

void	Server::sendResponse(int clientSocket, OutgoingData *response)
{
	std::cout << "sendResponse()   socket: " << clientSocket << "\n";
	
	_responses[clientSocket] = response;
	struct epoll_event epoll_ev;
	epoll_ev.events = EPOLLOUT;
	epoll_ev.data.fd = clientSocket;
	if (epoll_ctl(_epollFd, EPOLL_CTL_MOD, clientSocket, &epoll_ev) == -1)
		throw std::runtime_error("[sendResponse/epoll_ctl] failed to modify client socket events to EPOLLOUT");
}

void	Server::handleClientEvent(int clientSocket, uint32_t event)
{
	// getpeername() and getsockname() for debug/info:
	sockaddr_in	cliAddr, srvAddr;
	socklen_t	cliAddrLen = sizeof(cliAddr), srvAddrLen = sizeof(srvAddr);
	char cliIP[16], srvIP[16];
	int infoStatus = 0;

	infoStatus += getpeername(clientSocket, (sockaddr *)&cliAddr, &cliAddrLen);
	infoStatus += getsockname(clientSocket, (sockaddr *)&srvAddr, &srvAddrLen);
	if (infoStatus == 0)
	{
		inet_ntop(AF_INET, &cliAddr.sin_addr, cliIP, sizeof(cliIP));
		inet_ntop(AF_INET, &srvAddr.sin_addr, srvIP, sizeof(srvIP));
		std::cout << "handleClientEvent()  socket: " << clientSocket
			<< "  (" << cliIP << ":" << ntohs(cliAddr.sin_port)
			<< " <=> " << srvIP << ":" << ntohs(srvAddr.sin_port) << ")\n";
	}
	if (event & EPOLLIN)
	{
		char		buffer[IO_BUFFER_SIZE];
		
		try {
			ssize_t	bytesRead = safeRecv(clientSocket, buffer, sizeof(buffer), 0);
			if (bytesRead > 0)
			{
				std::cout << "   [EPOLLIN] " << bytesRead << " bytes received\n";
				_partialRequest[clientSocket].append(buffer, bytesRead);
				// Attention ici on s'arrete dès la fin des headers HTTP, on ne lit pas le body.
				// Si il y a un body il faudra le lire par la suite avant l'appel à processRequest() ou apres un
				// premier appel a processRequest qui permettra deja de refuser ou d'accepter
				// la requete en attendant de lire le body (requete POST).
				if (_partialRequest[clientSocket].find("\r\n\r\n") != std::string::npos)
				{
					processRequest(clientSocket, _partialRequest[clientSocket]);
					_partialRequest.erase(clientSocket);
				}
			}
			else if (bytesRead == 0)
			{
				std::cerr << "   [EPOLLIN] Error ? bytesRead == 0\n";
				closeConnection(clientSocket, event);
			}
		}
		catch (std::exception &e)
		{
			std::cerr << "   [EPOLLIN] Error receiving data from client : " << e.what() << std::endl;
			closeConnection(clientSocket, event);
		}
	}
	if (event & EPOLLOUT)
	{
		try {
			std::cout << "   [EPOLLOUT] -> handleOutgoingData()\n";
			handleOutgoingData(clientSocket);
		}
		catch (std::exception &e)
		{
			std::cerr << "Error sending data to client : " << e.what() << std::endl;
			closeConnection(clientSocket, event);	
		}
	}
	if (event & (EPOLLRDHUP | EPOLLHUP | EPOLLERR))
	{
		std::cout << "   [EPOLLRDHUP | EPOLLHUP | EPOLLERR] client disconnected\n";
		closeConnection(clientSocket, event);
	}
}

void Server::closeConnection(int clientSocket, uint32_t event)
{
	// Delete clientSocket from epoll instance _epollFd and close it
	epoll_ctl(_epollFd, EPOLL_CTL_DEL, clientSocket, NULL);
	close(clientSocket);
	
	switch (event)  // specific cleanup that depends on the epoll event (receiving, sending or error/hang up)
	{
	case EPOLLIN:
		_partialRequest.erase(clientSocket);
		break;
	case EPOLLOUT:
		if (_responses.find(clientSocket) != _responses.end())
		{
			delete _responses[clientSocket];
			_responses.erase(clientSocket);
		}
		break;
	case EPOLLRDHUP:
	case EPOLLHUP:
	case EPOLLERR:
		_partialRequest.erase(clientSocket);
		if (_responses.find(clientSocket) != _responses.end())
		{
			delete _responses[clientSocket];
			_responses.erase(clientSocket);
		}
	}
	// Finally erase client socket from _sockets map
	_sockets.erase(clientSocket);
}

void	Server::eventLoop()
{
	struct epoll_event	events[MAX_EVENTS];

	while (isAlive())
	{
		int nfds = epoll_wait(_epollFd, events, MAX_EVENTS, 100);  // timeout 100ms
		if (nfds == -1)
			throw std::runtime_error("[Server::run()] epoll_wait failed");
		for (int i = 0; i < nfds; i++)
		{
			if (_sockets[events[i].data.fd].isServer)
				handleNewConnection(events[i].data.fd);
			else
				handleClientEvent(events[i].data.fd, events[i].events);
		}
	}
}

void	Server::run()
{
	std::cout << "Server::run()   --- Press <Esc> to properly shutdown webserv ---\n\n";
	Terminal::disableEcho();
	Terminal::disableSignals();
	eventLoop();
	Terminal::enableEcho();
	Terminal::enableSignals();
}

bool	Server::isAlive()
{
	static bool isShutdownConfirmed = false;
	int c;
	
	if ((c = Terminal::getch()) == 27)  // <Esc> to shutdown webserver
	{
		if (isShutdownConfirmed) {
			shutDown();
			return false;
		}
		isShutdownConfirmed = true;
		std::cout << "\n  CAUTION : You are about to SHUTDOWN this magnificient WebServer.\n";
		std::cout << "  Are you ABSOLUTLY sure that that is what you REALLY want to do ?\n";
		std::cout << "  Press <Esc> to IRREVOCABLY confirm or any other key to abort.\n";
	}
	else if (c != -1 && isShutdownConfirmed)
	{
		std::cout << "\nShutdown aborted ; webserv is still alive :-)\n";
		isShutdownConfirmed = false;
	}
	return true;
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
