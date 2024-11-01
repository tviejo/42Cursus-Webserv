/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ade-sarr <ade-sarr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/08 12:47:41 by tviejo            #+#    #+#             */
/*   Updated: 2024/11/01 12:15:04 by ade-sarr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "webserv.hpp"
# include "HttpRequest.hpp"

/* Parse string request received from client
*/
bool	HTTPRequest::hasBody()
{
	std::map<std::string, std::string>::const_iterator it = _headers.find("Content-Length");
	if (it != _headers.end() && std::atol(it->second.c_str()) > 0)
		return true;	
	it = _headers.find("transfer-encoding");
	if (it != _headers.end() && it->second.find("chunked") != std::string::npos)
		return true;
	return false;
}

void	HTTPRequest::parseBody(std::istringstream& lineStream)
{
	std::map<std::string, std::string>::iterator lengthIt = _headers.find("Content-Length");
	if (lengthIt != _headers.end())
	{
		size_t length = std::atol(lengthIt->second.c_str());
		std::vector<char> buffer;
		buffer.resize(length);
	
		lineStream.read(&buffer[0], length);
		std::streamsize bytesRead = lineStream.gcount();
		if (bytesRead > 0)
			_body.assign(&buffer[0], bytesRead);
	}
	else
	{
		std::ostringstream	bodyStream;
		bodyStream << lineStream.rdbuf();
		_body = bodyStream.str();
	}
}

HTTPRequest::HTTPRequest(const std::string& request, t_server &server): _server(server)
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
			trimRef(value);
			_headers[key] = value;
		}
	}
	// A modifier/supprimer : le body devrait etre considéré comme un flux binaire
	// (ou bien tenir compte du content-type) et surtout faire l'objet d'un traitement
	// postérieur notemment dans le cas d'un POST avec un fichier quelconque.
	// Le constructeur HTTPRequest devrait etre appelé dès que l'on a recu la fin des entetes;
	// ce qui permettra déjà de décider si serveur refuse ou accepte la requete et
	// ensuite seuleument de recupérer le body avec par exemple un objet IngoingData
	// (classe qui reste à faire !)
	if (hasBody())
		parseBody(stream);
	_empty_string = "";
	extractQueryString();
}

void HTTPRequest::extractQueryString()
{
	size_t	sepPos = _uri.find_first_of('?');

	if (sepPos == std::string::npos)
	{
		_uriWithoutQString = Response::urlDecode(_uri);
		return ;
	}
	_uriWithoutQString = _uri.substr(0, sepPos);
	std::string queryStrs = _uri.substr(sepPos, _uri.size());
	if (queryStrs.length() > 0)
		queryStrs.erase(0, 1);
	while (queryStrs.length() > 0)
	{
		size_t sepPos = queryStrs.find_first_of('=');
		size_t endPos =  queryStrs.find_first_of('&');
		std::string key = queryStrs.substr(0, sepPos);
		std::string val = (sepPos == std::string::npos) ? ""
						: queryStrs.substr(sepPos + 1, endPos - sepPos - 1);
		_queryStrings[key] = val;
		std::cout << "        " << key << "='" << _queryStrings[key] << "'\n";
		if (endPos == std::string::npos)
			return;
		queryStrs.erase(0, endPos + 1);
	}
}

HTTPRequest::HTTPRequest(const HTTPRequest& copy): _server(copy._server)
{
    *this = copy;
}

HTTPRequest& HTTPRequest::operator=(const HTTPRequest& copy)
{
    if (this != &copy)
    {
		this->_server = copy._server;
		this->_method = copy._method;
		this->_uri = copy._uri;
		this->_uriWithoutQString = copy._uriWithoutQString;
		this->_httpVersion = copy._httpVersion;
		this->_headers = copy._headers;
		this->_queryStrings = copy._queryStrings;
		this->_body = copy._body;
		this->_empty_string = copy._empty_string;
    }
    return *this;
}

const std::string &HTTPRequest::get_method() const
{
	return _method;
}

const std::string	&HTTPRequest::getUri() const
{
	return _uri;
}

const std::string	&HTTPRequest::getUriWithoutQString() const
{
	return _uriWithoutQString;
}

const std::string	&HTTPRequest::getHttpVersion() const
{
	return _httpVersion;
}

const std::string	&HTTPRequest::getBody() const
{
	return _body;
}

const std::map<std::string, std::string>	&HTTPRequest::getHeaders() const
{
	return _headers;
}

const std::map<std::string, std::string>	&HTTPRequest::getQueryStrings() const
{
	return _queryStrings;
}

const std::string	&HTTPRequest::getQueryStrings(const std::string &key) const
{
	if (_queryStrings.find(key) == _queryStrings.end())
		return _empty_string;
	else
		return _queryStrings.at(key);
}

const std::string	&HTTPRequest::getFirstQueryString() const
{
	if (_queryStrings.empty())
		return _empty_string;
	return _queryStrings.begin()->second;
}

const t_server	&HTTPRequest::getServer() const
{
	return _server;
}

std::ostream & operator << (std::ostream &os, const HTTPRequest &req)
{
	return os << req.get_method() << " " << req.getUri() << " (" << req.getHttpVersion() << ")";
}

void	HTTPRequest::printRequest() const
{
	std::cerr << "Request: " << std::endl;
	std::cerr << "  method: " << _method << std::endl;
	std::cerr << "  uri: " << _uri << std::endl;
	std::cerr << "  uri without query string: " << _uriWithoutQString << std::endl;
	std::cerr << "  http version: " << _httpVersion << std::endl;
	std::cerr << "  Headers:\n";
	for (std::map<std::string, std::string>::const_iterator it = _headers.begin();
			it != _headers.end(); ++it)
	{
		std::cerr << "    " << it->first << ": " << it->second << std::endl;
	}
	std::cerr << "  Query strings:\n";
	for (std::map<std::string, std::string>::const_iterator it = _queryStrings.begin();
			it != _queryStrings.end(); ++it)
	{
		std::cerr << "    " << it->first << ": " << it->second << std::endl;
	}
	std::cerr << "  Body:\n" << _body << std::endl;
}
