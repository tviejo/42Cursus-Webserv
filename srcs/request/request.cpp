/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tviejo <tviejo@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/08 12:47:41 by tviejo            #+#    #+#             */
/*   Updated: 2024/10/08 12:47:54 by tviejo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "webserv.hpp"
# include "request.hpp"


HTTPRequest::HTTPRequest(const std::string& request)
{
	
}

HTTPRequest::HTTPRequest(const HTTPRequest& copy)
{
    *this = copy;
}

HTTPRequest& HTTPRequest::operator=(const HTTPRequest& copy)
{
    if (this != &copy)
    {
		this->_method = copy._method;
		this->_uri = copy._uri;
		this->_httpVersion = copy._httpVersion;
		this->_headers = copy._headers;
		this->_body = copy._body;
    }
    return *this;
}

