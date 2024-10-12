/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tviejo <tviejo@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/08 12:48:07 by tviejo            #+#    #+#             */
/*   Updated: 2024/10/08 16:16:40 by tviejo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "webserv.hpp"

class HTTPRequest
{
	private:
		std::string	_method;
		std::string	_uri;
		std::string	_httpVersion;
		std::unordered_map<std::string, std::string> _headers;
		std::string	_body;
	public:
	private:
		HTTPRequest();
	public:
		~HTTPRequest() {};
		HTTPRequest(const std::string& request);
		HTTPRequest(const HTTPRequest& copy);
		HTTPRequest& operator=(const HTTPRequest& copy);
};
