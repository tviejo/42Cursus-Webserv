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

std::string	HTTPRequest::cleanLineStream(std::istringstream& lineStream)
{
	std::string	line;
	std::string	cleanBody;
	while (std::getline(lineStream, line))
	{
		if (line.empty() == true && line[line.length() - 1] == '\r')
				line.erase(line.length() - 1); //remove trailing '\r'
	
		std::istringstream	iss(line);
		uint32_t			size;
		iss >> std::hex >> size;
		if (size == 0)
			break;
		
		std::string	chunk;
		char		c = '\0';
		chunk.reserve(size);
		for (uint32_t i = 0; i < size; i++)
			chunk += c;
		cleanBody += chunk;
		lineStream.ignore(2); //remove clrf after chunk, probably not needed
	}
	return cleanBody;
}

HTTPRequest::HTTPRequest(const std::string& request)
{
	std::istringstream	stream(request);
	std::string			request_line;
	std::getline(stream, request_line);
	std::istringstream	lineStream(request_line);
	
	lineStream >> _method >> _uri >> _httpVersion;
	while (std::getline(stream, request_line) && request_line != "\r")
	{
		size_t	sepPos = request_line.find(':');
		if (sepPos != std::string::npos)
		{
			std::string key = request_line.substr(0, sepPos);
			std::string value = request_line.substr(sepPos + 1);
			value.erase(0, value.find_last_not_of(" \t"));
			value.erase(value.find_last_not_of((" \t") + 1));
			_headers[key] = value;
		}
	}
	std::ostringstream	bodyStream;
	std::map<std::string, std::string>::iterator it = _headers.find("Transfer-Encoding");

	if (it != _headers.end() && it->second == "chunked")
		_body = cleanLineStream(lineStream);
	else
	{
		bodyStream << lineStream.rdbuf();
		_body = bodyStream.str();
	}
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

const std::string HTTPRequest::get_method() const
{
	return _method;
}
