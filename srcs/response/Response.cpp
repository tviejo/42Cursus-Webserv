/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tviejo <tviejo@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/08 12:48:44 by tviejo            #+#    #+#             */
/*   Updated: 2024/10/26 14:07:25 by tviejo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
#include <string>

std::map<std::string,std::string> Response::_contentTypeMap;

void Response::setupContentTypeMap()
{
	_contentTypeMap["html"] = "text/html";
	_contentTypeMap["css"] = "text/css";
	_contentTypeMap["js"] = "text/javascript";
	_contentTypeMap["txt"] = "text/plain";
	_contentTypeMap["jpg"] = "image/jpeg";
	_contentTypeMap["jpeg"] = "image/jpeg";
	_contentTypeMap["png"] = "image/png";
	_contentTypeMap["gif"] = "image/gif";
}

std::string	Response::makeResponseHeader(uint32_t status,
						 const std::string & statusMessage, 
						 const std::string & contentType,
						 size_t contentLength, int clientSocket)
{
	std::ostringstream	response;

	response << "HTTP/1.1 " << status << " " << statusMessage << "\r\n";
	if (clientSocket != -1)
		response << "Set-Cookie: ID=" << clientSocket << "; Path=/ \r\n";
	response << "Content-Type: " << contentType << "\r\n"; 
	response << "Content-Length: " << contentLength << "\r\n"; 
	//response << "Connection: Closed\r\n";
	response << "\r\n";
	return response.str();
}

OutgoingData *	Response::makeResponse(uint32_t status,
						 const std::string & statusMessage, 
						 const std::string & contentType,
						 const std::string & content)
{
	std::ostringstream	response;

	response << "HTTP/1.1 " << status << " " << statusMessage << "\r\n";
	response << "Content-Type: " << contentType << "\r\n"; 
	response << "Content-Length: " << content.length() << "\r\n"; 
	//response << "Connection: Closed\r\n";
	response << "\r\n";
	return new OutgoingData(response.str(), content);
}

/* Return first route that match 'uri'
 * if not found with full uri retry in a loop with uri without the last (1,2,3,...) folder(s)
 * it will ultimately match with "/" if this route exits or return NULL
*/
const t_route * Response::getRouteFromUri(const t_server & server, std::string uri)
{
	while (true) {
		//std::cout << "[getRouteFromUri():find loop] uri: " << uri << "\n";
		std::map<std::string, t_route>::const_iterator it;
		it = server.routes.find(uri.size() == 0 ? "/" : uri);
		if (it != server.routes.end())
			return &it->second;
		size_t lastSlashPos = uri.find_last_of("/");
		if (lastSlashPos != std::string::npos)
			uri.erase(lastSlashPos);
		else
			return NULL;
	}
}

std::string Response::getContentType(const std::string & uri)
{
	std::string ext = uri.substr(uri.find_last_of(".") + 1);
	return _contentTypeMap[ext];
}

OutgoingData * Response::handleGet(const t_server & server, const HTTPRequest & req, int clientSocket)
{
	std::string uri = req.getUriWithoutQString();
	const t_route *routeptr = getRouteFromUri(server, uri);
	if (routeptr == NULL) {
		std::cout << "     no route found from uri: " << uri << std::endl;
		return makeResponse(404, "Not Found", "text/plain", "404 Not Found");
	}
	const t_route &route = *routeptr;
	std::cout << "     route found: " << route.path << std::endl;
	if (route.methods.find(req.get_method()) == route.methods.end())
	{
		std::cout << "     Unauthorized method: " << req.get_method() << " for route: " << route.path << std::endl;
		return makeResponse(405, "Method Not Allowed", "text/plain", "405 Method Not Allowed");
	}
	if (route.path == "/cgi")
	{
		Cgi cgi("./cgi-bin/name.py", "GET", req.getQueryStrings("name"));
 		try
 		{
  	    	cgi.CgiHandler();
		}
		catch(const std::exception& e)
		{
			std::cerr << e.what() << '\n';
			return makeResponse(500, "Internal Server Error", "text/plain", "500 Internal Server Error");
		}
		return new OutgoingData(cgi.GetHeader(), cgi.GetResponse());
	}
	else if (route.path == "/time")
	{
		std::cerr << "\nTIME CGI\n\n";
		Cgi cgi("./cgi-bin/a.out", "GET", "");
 		try
 		{
  	    	cgi.CgiHandler();
		}
		catch(const std::exception& e)
		{
			std::cerr << e.what() << '\n';
			return makeResponse(500, "Internal Server Error", "text/plain", "500 Internal Server Error");
		}
		return new OutgoingData(cgi.GetHeader(), cgi.GetResponse());
	}
	else if (route.path == "/gallery")
	{
		Cgi cgi("./cgi-bin/gallery.cgi", "GET", "");
 		try
 		{
  	    	cgi.CgiHandler();
		}
		catch(const std::exception& e)
		{
			std::cerr << e.what() << '\n';
			return makeResponse(500, "Internal Server Error", "text/plain", "500 Internal Server Error");
		}
		std::cerr << cgi.GetHeader() << std::endl;
		std::cerr << cgi.GetResponse() << std::endl;
		return new OutgoingData(cgi.GetHeader(), cgi.GetResponse());
	}
	else {
		std::string filename;
		if (uri == route.path && route.autoindex)
			filename = route.directory + "/" + route.index;
		else
			filename = route.directory + "/" + uri.erase(0, route.path.length());
		if (isDirectory(filename)) {
			std::ostringstream entries;
			readDirectory(filename, entries);
			return makeResponse(200, "OK", "text/plain", entries.str());
		}
		ssize_t filesize = getFileSize(filename);
		std::cerr << "     filename: '" << filename << "'  filesize: " << filesize << "\n";
		if (filesize == -1) {
			filename = server.root + server.error;
			filesize = getFileSize(filename);
			if (filesize == -1)  // 404 error page is missing
				return makeResponse(404, "Not Found", "text/plain", "404 Not Found");
			std::string header = makeResponseHeader(404, "Not Found", getContentType("html"), filesize, clientSocket);
			return new OutgoingData(header, filename, true);
		}
		std::string header = makeResponseHeader(200, "OK", getContentType(uri), filesize, clientSocket);
		return new OutgoingData(header, filename, true);
	}
}

OutgoingData * Response::handlePost(const t_server & server, const HTTPRequest & req, int clientSocket)
{
	(void)server;
	(void)clientSocket;
	std::string uri = req.getUriWithoutQString();
	const t_route *routeptr = getRouteFromUri(server, uri);
	if (routeptr == NULL) {
		std::cout << "     no route found from uri: " << uri << std::endl;
		return makeResponse(404, "Not Found", "text/plain", "404 Not Found");
	}
	const t_route &route = *routeptr;
	std::cout << "     route found: " << route.path << std::endl;
	if (route.methods.find(req.get_method()) == route.methods.end())
	{
		std::cout << "     Unauthorized method: " << req.get_method() << " for route: " << route.path << std::endl;
		return makeResponse(405, "Method Not Allowed", "text/plain", "405 Method Not Allowed");
	}
	if (route.path == "/cgi")
	{
		Cgi cgi("./cgi-bin/name.py", "GET", "name=thomas");
 		try
 		{
  	    	cgi.CgiHandler();
		}
		catch(const std::exception& e)
		{
			std::cerr << e.what() << '\n';
			return makeResponse(500, "Internal Server Error", "text/plain", "500 Internal Server Error");
		}
		return new OutgoingData(cgi.GetHeader(), cgi.GetResponse());
	}
	else
	{
		return makeResponse(404, "Not Found", "text/plain", "404 Not Found");
	}
}

OutgoingData * Response::handleDelete(const t_server & server, const HTTPRequest & req, int clientSocket)
{
	(void)server;
	(void)clientSocket;
	std::cerr << "\nDELETE REQUEST\n\n";
	std::string uri = req.getUriWithoutQString();
	const t_route *routeptr = getRouteFromUri(server, uri);
	if (routeptr == NULL) {
		std::cerr << "     no route found from uri: " << uri << std::endl;
		return makeResponse(404, "Not Found", "text/plain", "404 Not Found");
	}
	std::cerr << uri << std::endl;
	const t_route &route = *routeptr;
	if (route.methods.find(req.get_method()) == route.methods.end())
	{
		std::cout << "     Unauthorized method: " << req.get_method() << " for route: " << route.path << std::endl;
		return makeResponse(405, "Method Not Allowed", "text/plain", "405 Method Not Allowed");
	}
	std::cerr << "     route found: " << route.path << std::endl;
	if (route.path == "/delete")
	{
		std::string file = "./www/html/uploadedFiles/" + req.getQueryStrings("file");
		std::cerr << "     file: " << file << std::endl;
 		if (std::remove(( "./www/html/uploadedFiles/" + req.getQueryStrings("file")).c_str()) != 0)
			return makeResponse(404, "Not Found", "text/plain", "404 Not Found");
		else
			return makeResponse(200, "OK", "text/plain", "200 OK");
	}
	else
		return makeResponse(404, "Not Found", "text/plain", "404 Not Found");
}