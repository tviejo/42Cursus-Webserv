/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tviejo <tviejo@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/07 12:56:20 by tviejo            #+#    #+#             */
/*   Updated: 2024/10/27 11:24:23 by tviejo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"
#include "config.hpp"

Config::Config(std::string conffile)
{
	try
	{
		parseConfig(conffile);
		for (size_t i = 0; i < servers.size(); i++)
		{
			if (!ServerIsValid(servers[i]))
				throw std::runtime_error("Error: server is not valid");
		}
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
}

Config::~Config()
{
}

void    Config::initRoute(t_route &route)
{
	route.path.clear();
	route.index.clear();
	route.methods.clear();
	route.upload.clear();
	route.directory.clear();
	route.cgi.clear();
	route.autoindex = false;
	route.max_body_size = 0;
}

void    Config::initServer(t_server &server)
{
	//server.ServerObject = NULL;
	server.server_name.clear();
	server.host.clear();
	server.port = 0;
	server.autoindex = false;
	server.root.clear();
	server.error.clear();
	server.max_body_size = 0;
	server.routes.clear();
}

void    Config::printConfig()
{
	for (size_t i = 0; i < servers.size(); i++)
	{
		std::cout << "server_name: " << servers[i].server_name << std::endl;
		std::cout << "host: " << servers[i].host << std::endl;
		std::cout << "port: " << servers[i].port << std::endl;
		std::cout << "autoindex: " << servers[i].autoindex << std::endl;
		std::cout << "root: " << servers[i].root << std::endl;
		std::cout << "error: " << servers[i].error << std::endl;
		std::cout << "max_body_size: " << servers[i].max_body_size << std::endl;
		for (std::map<std::string,t_route>::iterator it = servers[i].routes.begin(); it != servers[i].routes.end(); it++)
		{
			std::cout << std::endl;
			std::cout << "  path: " << it->second.path << std::endl;
			std::cout << "  index: " << it->second.index << std::endl;
			std::cout << "  methods: ";
			for (std::set<std::string>::iterator itm = it->second.methods.begin(); itm != it->second.methods.end(); itm++)
				std::cout << *itm << " ";
			std::cout << std::endl;
			std::cout << "  upload: " << it->second.upload << std::endl;
			std::cout << "  directory: " << it->second.directory << std::endl;
			std::cout << "  autoindex: " << it->second.autoindex << std::endl;
			std::cout << "  max_body_size: " << it->second.max_body_size << std::endl;
			std::cout << "  cgi: " << it->second.cgi << std::endl;
		}
		std::cout << std::endl << std::endl;
	}
}

void    Config::parseRoute(std::string &routePath, std::ifstream &file, t_route &route, t_server &server)
{
	std::string line;
	
	Config::initRoute(route);
	
	route.path = routePath;

	while (std::getline(file, line))
	{
		if (line.find("}") != std::string::npos)
			break;
		/*if (line.find("path = ") != std::string::npos)
			route.path = line.substr(line.find("path = ") + 7);*/
		if (line.find("index = ") != std::string::npos)
			route.index = line.substr(line.find("index =") + 8);
		if (line.find("methods = ") != std::string::npos)
		{
			std::string methods = line.substr(line.find("methods =") + 10);
			std::string method;
			for (size_t i = 0; i < methods.size(); i++)
			{
				if (methods[i] == ' ')
				{
					route.methods.insert(method);
					method.clear();
				}
				else
					method += methods[i];
			}
			route.methods.insert(method);
		}
		if (line.find("upload = ") != std::string::npos)
			route.upload = server.root + trim(line.substr(line.find("=") + 1));
		/*if (line.find("directory = ") != std::string::npos)
			route.directory = line.substr(line.find("directory = ") + 12);*/
		if (line.find("root = ") != std::string::npos)
			route.directory = server.root + trim(line.substr(line.find("=") + 1));
		if (line.find("autoindex = ") != std::string::npos)
			route.autoindex = line.substr(line.find("autoindex = ") + 12) == "on" ? true : false;
		else
			route.autoindex = server.autoindex; // if autoindex is not defined for route get the global server value
		if (line.find("max_body_size = ") != std::string::npos)
			route.max_body_size = std::atol(line.substr(line.find("max_body_size = ") + 16).c_str());
		if (line.find("cgi = ") != std::string::npos)
			route.cgi = line.substr(line.find("cgi = ") + 6);
		else
			route.max_body_size = server.max_body_size; // if max_body_size is not defined for route get the global server value
	}
}

void    Config::parseServer(std::ifstream &file, t_server &server)
{
	Config::initServer(server);
	std::string line;
	while (std::getline(file, line))
	{
		if (line.find("}") != std::string::npos)
			break;
		if (line.find("server_name = ") != std::string::npos)
			server.server_name = line.substr(line.find("server_name = ") + 14);
		if (line.find("host = ") != std::string::npos)
			server.host = line.substr(line.find("host = ") + 7);
		if (line.find("port = ") != std::string::npos)
			server.port = std::atol(line.substr(line.find("port = ") + 7).c_str());
		if (line.find("autoindex = ") != std::string::npos)
			server.autoindex = line.substr(line.find("autoindex = ") + 12) == "on" ? true : false;
		if (line.find("root = ") != std::string::npos)
			server.root = line.substr(line.find("root = ") + 7);
		if (line.find("error = ") != std::string::npos)
			server.error = line.substr(line.find("error = ") + 8);
		if (line.find("max_body_size = ") != std::string::npos)
			server.max_body_size = std::atol(line.substr(line.find("max_body_size = ") + 16).c_str());
		if (line.find("route = ") != std::string::npos)
		{
			t_route route;
			std::string path = line.substr(line.find("=") + 1);
			parseRoute(trimRef(path, "\" \t\n\r\f\v"), file, route, server);
			//server.routes.push_back(route);
			server.routes[route.path] = route;
		}
	}
}

void    Config::parseConfig(std::string conffile)
{
	std::ifstream file(conffile.c_str());
	if (!file.is_open())
		throw std::runtime_error("Error: could not open file");
	std::string line;
	while (std::getline(file, line))
	{
		if (line.find("server") != std::string::npos)
		{
			t_server server;
			parseServer(file, server);
			servers.push_back(server);
		}
	}
}

bool    Config::ServerIsValid(t_server &server)
{
	if (server.host.empty() || server.port == 0 || server.root.empty() || server.error.empty())
	{
		throw std::runtime_error("Error: server is not valid");
		return false;
	}
	for (std::map<std::string,t_route>::iterator it = server.routes.begin(); it != server.routes.end(); it++)
	{
		if (it->second.methods.empty() || it->second.index.empty())
		{
			throw std::runtime_error("Error: route is not valid");
			return false;
		}
	}
	return true;
}

const std::vector<t_server>& Config::getServers() const
{
	return servers;
}
