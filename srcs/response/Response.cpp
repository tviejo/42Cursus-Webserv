/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ade-sarr <ade-sarr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/08 12:48:44 by tviejo            #+#    #+#             */
/*   Updated: 2024/10/18 21:30:32 by ade-sarr         ###   ########.fr       */
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

std::string	Response::makeResponse(uint32_t status,
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
	response << content;
	return response.str();
}

const t_route & Response::getRouteFromUri(const t_server & server, std::string uri)
{
	(void)server;
	(void)uri;
	return server.routes.begin()->second;
}

std::string Response::getContentType(const std::string & uri)
{
	std::string ext = uri.substr(uri.find_last_of(".") + 1);
	return _contentTypeMap[ext];
}

std::string	Response::handleGet(const t_server & server, const HTTPRequest & req)
{
	//std::string content = getContentFromReqUri(req.getUri());
	/*std::string content = "<HTML><HEAD></HEAD><BODY><H1>Hello Big Title !</H1><P><CODE>URI: " + req.getUri()
		+ "</CODE><P>Hello HTML page content !<P>Vive le père Noël.</BODY></HTML>";*/
	
	const t_route &route = getRouteFromUri(server, req.getUri());
	if (route.methods.find(req.get_method()) == route.methods.end())
	{
		std::cout << "Unauthorized method: " << req.get_method() << " for route: " << route.path << std::endl;
		return makeResponse(405, "Method Not Allowed", "text/plain", "405 Method Not Allowed");
	}
	std::string content;
	std::ifstream ifs((route.directory + req.getUri()).c_str(), std::ios_base::in);
	std::ostringstream oss;
	while (ifs) {
		std::getline(ifs, content);
		oss << content;
	}
	content = oss.str();
	if (content.empty() == true)
		return makeResponse(404, "Not Found", "text/plain", "404 Not Found");
	else
		return makeResponse(200, "OK", getContentType(req.getUri()), content);
		//return makeResponse(200, "OK", "text/html; charset=utf-8", content);
}

std::string	Response::handlePost(const t_server & server, const HTTPRequest & req)
{
	(void)server;
	(void)req;
	return ("dummy");
}

std::string	Response::handleDelete(const t_server & server, const HTTPRequest & req)
{
	(void)server;
	(void)req;
	return ("dummy");
}
