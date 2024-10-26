/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ade-sarr <ade-sarr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/08 12:50:59 by tviejo            #+#    #+#             */
/*   Updated: 2024/10/26 10:53:12 by ade-sarr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HPP
# define CGI_HPP

# include "webserv.hpp"
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
		std::string GetRespHeader() { return _header; }
		std::string GetRespBody() { return _response; }
		std::string createHeader(size_t status, std::string message, std::string contentType, size_t contentLength, std::string cookie);
		class OutgoingData * makeResponse();
};

#endif
