/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tviejo <tviejo@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/07 12:57:38 by tviejo            #+#    #+#             */
/*   Updated: 2024/10/27 11:20:09 by tviejo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
# define CONFIG_HPP

# include "webserv.hpp"



typedef struct route
{
	std::string         path;       // route path ("/", "/top-secret")
	std::string         index;      // index file
	std::set<std::string> methods;  // allowed methods (GET, POST, DELETE)
	std::string         upload;     // upload directory
	std::string         directory;  // server directory for route (exple: "/srv/www/html" for "/")
	std::string         cgi;		// cgi path and extension
	bool                autoindex;
	size_t              max_body_size;
} t_route;

typedef struct server
{
	//class Server*       ServerObject;  // lately initilized in Server constructor
	std::string         server_name;
	std::string         host;
	int                 port;
	bool                autoindex;
	std::string         root;   // server root directory (exple: "/srv/www")
	std::string         error;  // default error page
	size_t              max_body_size;
	//std::vector<t_route> routes;
	std::map<std::string, t_route> routes;  // map<path, t_route>
} t_server;

class Config
{
	private:
		std::vector<t_server> servers;
		
		void    parseConfig(std::string file);
		void    parseServer(std::ifstream &file, t_server &server);
		void    parseRoute(std::string &routePath, std::ifstream &file, t_route &route, t_server &server);
		void    initServer(t_server &server);
		void    initRoute(t_route &route);
		bool    ServerIsValid(t_server &server);
	public:
		Config(std::string file);
		~Config();
		void    printConfig();
		const std::vector<t_server>& getServers() const;
};


#endif
