/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ade-sarr <ade-sarr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/08 12:48:44 by tviejo            #+#    #+#             */
/*   Updated: 2024/10/17 14:45:59 by ade-sarr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "response.hpp"

std::string	makeResponse(uint32_t status,
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

const t_route & getRouteFromUri(const t_server & server, std::string uri)
{
	(void)server;
	(void)uri;
	return server.routes[0];
}

std::string	handleGetResponse(const t_server & server, const HTTPRequest & req)
{
	//std::string content = getContentFromReqUri(req.getUri());
	std::string content = "<HTML><HEAD></HEAD><BODY><H1>Hello Big Title !</H1><P><CODE>" + req.getUri()
		+ "</CODE><P>Hello HTML page content !<P>Vive le père Noël.</BODY></HTML>";
	
	const t_route &route = getRouteFromUri(server, req.getUri());
	if (route.methods.find(req.get_method()) == route.methods.end())
		throw std::runtime_error("unauthorized method: " + req.get_method() + " for route: " + route.path);
	
	if (content.empty() == true)
		return makeResponse(404, "Not Found", "text/plain", "404 Not Found");
	else
		return makeResponse(200, "OK", "text/html; charset=utf-8", content);
}

std::string	handlePostResponse(const t_server & server, const HTTPRequest & req)
{
	(void)server;
	(void)req;
	return ("dummy");
}

std::string	handleDeleteResponse(const t_server & server, const HTTPRequest & req)
{
	(void)server;
	(void)req;
	return ("dummy");
}
