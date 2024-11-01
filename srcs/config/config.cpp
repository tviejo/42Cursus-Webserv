/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ade-sarr <ade-sarr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/07 12:56:20 by tviejo            #+#    #+#             */
/*   Updated: 2024/11/01 13:49:21 by ade-sarr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"
#include "config.hpp"


bool	CheckIfAllPortAreDifferent(std::vector<t_server> servers)
{
	for (size_t i = 0; i < servers.size(); i++)
	{
		for (size_t j = i + 1; j < servers.size(); j++)
		{
			if (servers[i].port == servers[j].port)
				return false;
		}
	}
	return true;
}

Config::Config(std::string conffile)
{
		parseConfig(conffile);
		for (size_t i = 0; i < servers.size(); i++)
		{
			if (!ServerIsValid(servers[i]))
				throw std::runtime_error("Error: server is not valid");
		}
		if (!CheckIfAllPortAreDifferent(servers))
			throw std::runtime_error("Error: ports are not unique");
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
	route.dir_listing = false;
	route.max_body_size = 0;
}

void    Config::initServer(t_server &server)
{
	server.server_name.clear();
	server.host.clear();
	server.port = 0;
	server.autoindex = false;
	server.dir_listing = false;
	server.root.clear();
	server.error.clear();
	server.max_body_size = 0;
	server.routes.clear();
	server.redirs.clear();
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
		for (std::map<std::string,std::string>::iterator it = servers[i].redirs.begin(); it != servers[i].redirs.end(); it++)
		{
			std::cout << "redirection: " << it->first << " => " << it->second << std::endl;
		}
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
		
		if (line.find("index =") != std::string::npos)
			route.index = trim(line.substr(line.find("=") + 1));
		
		if (line.find("methods =") != std::string::npos)
		{
			std::string methods = trim(line.substr(line.find("=") + 1));
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
		if (line.find("upload =") != std::string::npos)
			route.upload = server.root + trim(line.substr(line.find("=") + 1));
		
		if (line.find("directory =") != std::string::npos)
			route.directory = server.root + trim(line.substr(line.find("=") + 1));
		
		if (line.find("autoindex =") != std::string::npos)
			route.autoindex = trim(line.substr(line.find("=") + 1)) == "on";
		else
			route.autoindex = server.autoindex; // if autoindex is not defined for route get the global server value
		
		if (line.find("dir_listing =") != std::string::npos)
			route.dir_listing = trim(line.substr(line.find("=") + 1)) == "on";
		else
			route.dir_listing = server.dir_listing; // if dir_listing is not defined for route get the global server value
		
		if (line.find("max_body_size =") != std::string::npos)
			route.max_body_size = std::atol(line.substr(line.find("=") + 1).c_str());
		else
			route.max_body_size = server.max_body_size; // if max_body_size is not defined for route get the global server value
		
		if (line.find("cgi =") != std::string::npos)
			route.cgi = trim(line.substr(line.find("=") + 1));
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
		if (line.find("server_name =") != std::string::npos)
			server.server_name = trim(line.substr(line.find("=") + 1));
		if (line.find("host =") != std::string::npos)
			server.host = trim(line.substr(line.find("=") + 1));
		if (line.find("port =") != std::string::npos)
			server.port = std::atol(line.substr(line.find("=") + 1).c_str());
		if (line.find("autoindex =") != std::string::npos)
			server.autoindex = trim(line.substr(line.find("=") + 1)) == "on";
		if (line.find("dir_listing =") != std::string::npos)
			server.dir_listing = trim(line.substr(line.find("=") + 1)) == "on";
		if (line.find("root =") != std::string::npos)
			server.root = trim(line.substr(line.find("=") + 1));
		if (line.find("error =") != std::string::npos)
			server.error = trim(line.substr(line.find("=") + 1));
		if (line.find("max_body_size =") != std::string::npos)
			server.max_body_size = std::atol(line.substr(line.find("=") + 1).c_str());
		
		if (line.find("redirection ") != std::string::npos)
		{
			try {
				std::string routePath = line.substr(12, line.find_first_of('=') - 12);
				std::string targetPath = line.substr(line.find_first_of('=') + 1);
				server.redirs[trimRef(routePath, "\" \t\n\r\f\v")] = trimRef(targetPath, "\" \t\n\r\f\v");
			} catch (std::exception &e) {
				std::cerr << "[Config::parseServer] invalid redirection (ignored): " << e.what() << "\n";
			}
		}
		if (line.find("route =") != std::string::npos)
		{
			t_route route;
			std::string path = line.substr(line.find("=") + 1);
			parseRoute(trimRef(path, "\" \t\n\r\f\v"), file, route, server);
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

bool   Config::MethodIsValid(std::set<std::string> methods)
{
	for (std::set<std::string>::iterator it = methods.begin(); it != methods.end(); it++)
	{
		if (*it != "GET" && *it != "POST" && *it != "DELETE")
		{
			return false;
		}
	}
	return true;
}

bool	Config::RouteISValid(t_route &route)
{
	if (route.path.empty() || route.directory.empty())
	{
		throw std::runtime_error("Error: route path is not valid");
		return false;
	}
	if (MethodIsValid(route.methods) == false)
	{
		throw std::runtime_error("Error: route methods are not valid");
		return false;
	}
	return true;
}

bool    Config::ServerIsValid(t_server &server)
{
	if (server.host.empty() || server.port == 0 || server.root.empty() || server.server_name.empty())
	{
		throw std::runtime_error("Error: server is not valid");
		return false;
	}
	if (server.error.empty())
	{
		server.error = "/html/errors/errors.html";
	}
	if (server.max_body_size == 0)
	{
		server.max_body_size = 5000000;
	}
	if (server.max_body_size > 100000000)
	{
		throw std::runtime_error("Error: max_body_size is too big");
		return false;
	}
	for (std::map<std::string,t_route>::iterator it = server.routes.begin(); it != server.routes.end(); it++)
	{
		if (!RouteISValid(it->second))
			return false;
	}
	return true;
}
const std::vector<t_server>& Config::getServers() const
{
	return servers;
}