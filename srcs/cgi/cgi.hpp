/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tviejo <tviejo@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/08 12:50:59 by tviejo            #+#    #+#             */
/*   Updated: 2024/10/25 10:19:07 by tviejo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HPP
# define CGI_HPP

# include <iostream>
# include <sstream>
# include <string>
# include <string>
# include <cstring>
# include <cstdlib>
# include <vector>
# include <unistd.h>
# include <stdlib.h>
# include <sys/types.h>
# include <sys/wait.h>
# include "webserv.hpp"

class Cgi
{
    private:
        Cgi();
        Cgi(const Cgi &copy);
        Cgi &operator=(const Cgi &copy);
        char **getEnvp();
        void    deleteEnvp(char **envp);
        std::string _env;
        std::string _path;
        std::string _method;
        std::string _type;
        std::string _response;
        std::string _header;
        size_t _contentLength;
        bool _isDone;
    public:
        Cgi(std::string path, std::string method, std::string info);
        ~Cgi();
        void execute();
        void CgiHandler();
        std::string GetHeader() { return _header; }
        std::string GetResponse() { return _response; }
        
};

#endif