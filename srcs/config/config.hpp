/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ade-sarr <ade-sarr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/07 12:57:38 by tviejo            #+#    #+#             */
/*   Updated: 2024/10/30 14:38:39 by ade-sarr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
# define CONFIG_HPP

# include "webserv.hpp"



typedef struct route
{
	std::string         path;          // route path ("/", "/top-secret")
	std::string         index;         // index file (exple: 'index.html')
	std::set<std::string> methods;     // allowed methods (GET, POST, DELETE)
	std::string         upload;        // upload directory
	std::string         directory;     // directory for route: made from server.root + route.directory from config file
									   //     exple: www/my-route-dir ('www' + '/my-route-dir')
	std::string         cgi;		   // cgi path and extension
	bool                autoindex;     // if true (on) send index file if GET request is route path
	bool				dir_listing;   // allow directory listing
	size_t              max_body_size; // the (in)famous one ! :-)
} t_route;

typedef struct server
{
	std::string         server_name;
	std::string         host;
	int                 port;
	bool                autoindex;     // send index file if GET request is route path
	bool				dir_listing;   // allow directory listing
	std::string         root;          // server root directory (absolute or relative exple: '/srv/www', 'www')
	std::string         error;         // default error page
	size_t              max_body_size;
	std::map<std::string, t_route> routes;  // map<path, t_route>
	std::map<std::string, std::string> redirs;  // redirections (send 301 HTTP response)
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
