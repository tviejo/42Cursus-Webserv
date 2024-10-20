/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ade-sarr <ade-sarr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/08 12:48:44 by tviejo            #+#    #+#             */
/*   Updated: 2024/10/20 04:01:10 by ade-sarr         ###   ########.fr       */
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
						 size_t contentLength)
{
	std::ostringstream	response;

	response << "HTTP/1.1 " << status << " " << statusMessage << "\r\n";
	response << "Content-Type: " << contentType << "\r\n"; 
	response << "Content-Length: " << contentLength << "\r\n"; 
	response << "Connection: Closed\r\n";
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
	response << "Connection: Closed\r\n";
	response << "\r\n";
	//response << content;
	//return response.str();
	return new OutgoingData(response.str(), content);
}

const t_route & Response::getRouteFromUri(const t_server & server, std::string uri)
{
	// TODO : return first route that match uri 
	// if not found retry in a loop with uri without the last (1,2,3,...) folder(s)
	// it will ultimately match with "/"
	(void)server;
	(void)uri;
	return server.routes.begin()->second;
}

std::string Response::getContentType(const std::string & uri)
{
	std::string ext = uri.substr(uri.find_last_of(".") + 1);
	return _contentTypeMap[ext];
}

OutgoingData * Response::handleGet(const t_server & server, const HTTPRequest & req)
{
	const t_route &route = getRouteFromUri(server, req.getUri());
	if (route.methods.find(req.get_method()) == route.methods.end())
	{
		std::cout << "Unauthorized method: " << req.get_method() << " for route: " << route.path << std::endl;
		return makeResponse(405, "Method Not Allowed", "text/plain", "405 Method Not Allowed");
	}
	if (route.path == "/cgi")
	{
		// return new OutgoingData(cgiRespHeader, cgiRespBody);
		return makeResponse(404, "Not Found", "text/plain", "404 Not Found");
	}
	else {
		std::string filename;
		if (req.getUri() == route.path && route.autoindex)
			filename = route.directory + "/" + route.index;
		else
			filename = route.directory + req.getUri();
		if (isDirectory(filename)) {
			std::ostringstream entries;
			readDirectory(filename, entries);
			return makeResponse(200, "OK", "text/plain", entries.str());
		}
		ssize_t filesize = getFileSize(filename);
		std::cout << "         filename: '" << filename << "'  filesize: " << filesize << "\n";
		if (filesize == -1) {
			filename = server.root + server.error;
			filesize = getFileSize(filename);
			if (filesize == -1)  // 404 error page is missing
				return makeResponse(404, "Not Found", "text/plain", "404 Not Found");
			std::string header = makeResponseHeader(404, "Not Found", getContentType("html"), filesize);
			return new OutgoingData(header, filename, true);
		}
		std::string header = makeResponseHeader(200, "OK", getContentType(req.getUri()), filesize);
		return new OutgoingData(header, filename, true);
	}
}

OutgoingData * Response::handlePost(const t_server & server, const HTTPRequest & req)
{
	(void)server;
	(void)req;
	return NULL;
}

OutgoingData * Response::handleDelete(const t_server & server, const HTTPRequest & req)
{
	(void)server;
	(void)req;
	return NULL;
}
