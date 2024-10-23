/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ade-sarr <ade-sarr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/08 12:47:41 by tviejo            #+#    #+#             */
/*   Updated: 2024/10/23 10:29:38 by ade-sarr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "webserv.hpp"
# include "HttpRequest.hpp"

/* Parse string request received from client
*/
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
	extractQueryString();

	// A modifier/supprimer : le body devrait etre considéré comme un flux binaire (ou bien
	// tenir compte du content-type) et surtout faire l'objet d'un traitement
	// postérieur notemment dans le cas d'un POST avec un fichier quelconque.
	// le constructeur HTTPRequest devrait etre appelé dès que l'on a recu la fin des entetes;
	// ce qui permet déjà de décider si serveur refuse ou accepte la requete et
	// ensuite seuleument on recupère le body (avec par exemple un objet de classe
	// IngoingData qui reste à créer !)
	std::ostringstream	bodyStream;
	bodyStream << lineStream.rdbuf();
	_body = bodyStream.str();
}

void HTTPRequest::extractQueryString()
{
	// TODO : 
	// renseigner _uriWithoutQString;
	// renseigner _queryStrings;
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
		this->_uriWithoutQString = copy._uriWithoutQString;
		this->_httpVersion = copy._httpVersion;
		this->_headers = copy._headers;
		this->_queryStrings = copy._queryStrings;
		this->_body = copy._body;
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

std::ostream & operator << (std::ostream &os, const HTTPRequest &req)
{
	return os << req.get_method() << " " << req.getUri() << " (" << req.getHttpVersion() << ")";
}
