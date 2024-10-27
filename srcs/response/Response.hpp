/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tviejo <tviejo@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/08 12:48:33 by tviejo            #+#    #+#             */
/*   Updated: 2024/10/27 13:10:26 by tviejo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "webserv.hpp"
#include <ostream>

class Response
{
private:
	static std::map<std::string,std::string> _contentTypeMap;

public:	
	static const t_route *	getRouteFromUri(const t_server &, std::string uri);
	static OutgoingData *	handleGet(const t_server &, const HTTPRequest &, int clientSocket, Server &serverObj);
	static OutgoingData *	handlePost(const t_server &, const HTTPRequest &, int clientSocket);
	static OutgoingData *	handleDelete(const t_server &, const HTTPRequest &, int clientSocket);
	static std::string		makeResponseHeader(uint32_t status,
										 const std::string & statusMessage,
										 const std::string & contentType,
									 	 size_t contentLength, int clientSocket);
	static OutgoingData *	makeResponse(uint32_t status,
										 const std::string & statusMessage,
										 const std::string & contentType,
									 	 const std::string & content,
										 const std::string & addHeader = "");
	static OutgoingData *	makeErrorResponse(uint32_t status,
												const std::string &statusMessage,
												const t_server &server,
												int clientSocket);
	static OutgoingData *	makeErrorResponse(uint32_t status,
												const std::string &statusMessage,
												std::string root,
												std::string error,
												int clientSocket);
	static void				setupContentTypeMap();
	static std::string		getContentType(const std::string & uri);
};
