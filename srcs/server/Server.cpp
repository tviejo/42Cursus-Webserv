
#include "Server.hpp"


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

void	Server::start()
{
	setupSockets();
	initEpoll();
	eventLoop();
}
