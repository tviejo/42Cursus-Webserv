/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ade-sarr <ade-sarr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/08 12:50:59 by tviejo            #+#    #+#             */
/*   Updated: 2024/11/03 17:33:22 by ade-sarr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HPP
# define CGI_HPP

# include "webserv.hpp"
# include <string.h>
# include <sys/wait.h>

class Cgi
{
	private:
		//Cgi();
		//Cgi(const Cgi &copy);
		//Cgi &operator=(const Cgi &copy);
		char **getEnvp();
		void    deleteEnvp(char **envp);

		void execute();
		void CgiHandler();
		std::string GetHeader() { return _header; }
        std::string GetResponse() { return _response; }
		std::string createHeader(size_t status, std::string message, std::string contentType, size_t contentLength, std::string cookie);
		int			_clientId;
		const class HTTPRequest &_request;
		std::string _env;
		int 		_nbVarsEnv;
		std::string _execPath;
		std::string _response;
		std::string _header;
		size_t _contentLength;
		bool _isUserNameCgi;
		bool _isDone;
	
	public:
		Cgi(std::string execPath, const class HTTPRequest &req, int clientId);
		class OutgoingData *handleCgi();
		//~Cgi();
};

class TimeoutException : public std::exception
{
	public:
		virtual const char *what() const throw() { return "Timeout"; }
		virtual ~TimeoutException() throw() {}
};

#endif
