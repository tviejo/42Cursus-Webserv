/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ade-sarr <ade-sarr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/08 12:48:07 by tviejo            #+#    #+#             */
/*   Updated: 2024/11/01 07:57:42 by ade-sarr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "webserv.hpp"
#include <sstream>

class HTTPRequest
{
	private:
		t_server &  _server;
		std::string	_method;
		std::string	_uri;
		std::string	_uriWithoutQString;
		std::map<std::string, std::string> _queryStrings;
		std::string	_httpVersion;
		std::map<std::string, std::string> _headers;
		std::string	_body;
		std::string	_empty_string;
	public:
	private:
		HTTPRequest();
		void extractQueryString();
	public:
		~HTTPRequest() {};
		HTTPRequest(const std::string& request, t_server &server);
		HTTPRequest(const HTTPRequest& copy);
		HTTPRequest& operator=(const HTTPRequest& copy);
		const std::string	&get_method() const;
		const std::string	&getUri() const;
		const std::string	&getUriWithoutQString() const;
		const std::map<std::string, std::string> &getQueryStrings() const;
		const std::string	&getHttpVersion() const;
		const std::string	&getBody() const;
		const std::map<std::string, std::string>	&getHeaders() const;
		//const std::map<std::string, std::string>	&getQueryStrings() const;
		const std::string	&getQueryStrings(const std::string &key) const;
		const std::string	&getFirstQueryString() const;
		const t_server		&getServer() const;
		bool				hasBody();
		void				parseBody(std::istringstream& lineStream);
		void printRequest() const;
};

std::ostream & operator << (std::ostream &, const HTTPRequest &);
