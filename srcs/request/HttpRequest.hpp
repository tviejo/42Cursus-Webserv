/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ade-sarr <ade-sarr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/08 12:48:07 by tviejo            #+#    #+#             */
/*   Updated: 2024/11/03 17:27:21 by ade-sarr         ###   ########.fr       */
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
		map_str 	_queryStrings;
		map_str 	_form;
		std::string	_httpVersion;
		map_str 	_headers;
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
		const std::string	&getHttpVersion() const;
		const std::string	&getBody() const;
		const map_str		&getHeaders() const;
		const map_str		&getQueryStrings() const;
		const std::string	getQueryStrings(const std::string &key, bool decoded = false) const;
		const std::string	getFirstQueryString(bool decoded = false) const;
		const t_server		&getServer() const;
		bool				hasBody();
		void				parseBody(std::istringstream& lineStream);
		map_str				&getForm() { return _form; }
		const map_str		&getForm() const { return _form; }
		const std::string	&getFormField(const std::string &key) const;
		const std::string	&getFirstFormField() const;
		void 				printRequest(bool printHeaders = true, bool printQueryStrs = true, bool printBody = false) const;
};

std::ostream & operator << (std::ostream &, const HTTPRequest &);
