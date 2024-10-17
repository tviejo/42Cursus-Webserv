/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ade-sarr <ade-sarr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/08 12:48:44 by tviejo            #+#    #+#             */
/*   Updated: 2024/10/17 02:24:32 by ade-sarr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "response.hpp"

std::string	makeResponse(uint32_t status, const std::string& statusMessage, 
						 const std::string& contentType, const std::string& content)
{
	std::ostringstream	response;

	response << "HTTP/1.1" << status << " " << statusMessage << "\r\n";
	response << "Content-type: " << contentType << "\r\n"; 
	response << "Content-length: " << content.length() << "\r\n"; 
	response << "\r\n";
	response << content;
	//response << "\r\n";
	return response.str();
}

std::string	handleGetResponse(const HTTPRequest& req)
{
	//std::string content = getContentFromReqUri(req.getUri());
	std::string content = "<HTML><HEAD></HEAD><BODY><H1>Hello Big Title !</H1><P><CODE>" + req.getUri()
		+ "</CODE><P>Hello HTML page content !<P>Vive le père Noël.</BODY></HTML>";
	
	if (content.empty() == true)
		return makeResponse(404, "Not Found", "text/plain", "404 Not Found");
	else
		return makeResponse(200, "OK", "text/html; charset=utf-8", content);
}

std::string	handlePostResponse(const HTTPRequest& req)
{
	(void)req;
	return ("dummy");
}

std::string	handleDeleteResponse(const HTTPRequest& req)
{
	(void)req;
	return ("dummy");
}
