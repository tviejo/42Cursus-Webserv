/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ade-sarr <ade-sarr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/08 12:48:07 by tviejo            #+#    #+#             */
/*   Updated: 2024/10/14 17:56:51 by ade-sarr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "webserv.hpp"
#include <sstream>

class HTTPRequest
{
	private:
		std::string	_method;
		std::string	_uri;
		std::string	_httpVersion;
		std::map<std::string, std::string> _headers;
		std::string	_body;
	public:
	private:
		HTTPRequest();
	public:
		~HTTPRequest() {};
		HTTPRequest(const std::string& request);
		HTTPRequest(const HTTPRequest& copy);
		HTTPRequest& operator=(const HTTPRequest& copy);
		const std::string	&get_method() const;
		const std::string	&getUri() const;
		const std::string	&getHttpVersion() const;
		const std::string	&getBody() const;
		const std::map<std::string, std::string>	&getHeaders() const;
};
