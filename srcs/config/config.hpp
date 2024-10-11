/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tviejo <tviejo@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/07 12:57:38 by tviejo            #+#    #+#             */
/*   Updated: 2024/10/08 14:04:11 by tviejo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
# define CONFIG_HPP

# include "webserv.hpp"



typedef struct route
{
    std::string         path;
    std::string         index;
    std::set<std::string> methods;
    std::string         upload;
    std::string         directory;
    bool                autoindex;
    double              max_body_size;
} t_route;

typedef struct server
{
    std::string         server_name;
    std::string         host;
    int                 port;
    bool                autoindex;
    std::string         root;
    std::string         error;
    int                 max_body_size;
    std::vector<t_route> routes;
} t_server;

class Config
{
    private:
        std::vector<t_server> servers;
        void    parseConfig(std::string file);
        void    parseServer(std::ifstream &file, t_server &server);
        void    parseRoute(std::ifstream &file, t_route &route);
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
