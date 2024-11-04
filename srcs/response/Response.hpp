/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ade-sarr <ade-sarr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/08 12:48:33 by tviejo            #+#    #+#             */
/*   Updated: 2024/11/03 17:08:39 by ade-sarr         ###   ########.fr       */
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
	static OutgoingData *	handleGet(const HTTPRequest &, int clientSocket, Server &serverObj);
	static OutgoingData *	handlePost(HTTPRequest &, int clientSocket);
	static OutgoingData *	handleDelete(const HTTPRequest &, int clientSocket);
	
	static std::string		makeResponseHeader(uint32_t status,
										 const std::string & statusMessage,
										 const std::string & contentType,
									 	 size_t contentLength, int ID_cookie);
	static OutgoingData *	makeResponse(uint32_t status,
										 const std::string & statusMessage,
										 const std::string & contentType,
									 	 const std::string & content,
										 const std::string & addHeader = "");
	static OutgoingData *	makeErrorResponse(uint32_t status,
												const std::string &statusMessage,
												const t_server &server,
												int clientId);
	
	static void				setupContentTypeMap();
	static std::string		getContentType(const std::string & uri);
	
	static OutgoingData*	handleTextPost(const HTTPRequest& req, const size_t maxBodySize);
	static OutgoingData*	handleFileUpload(const HTTPRequest& req, const size_t maxBodySize);
	static OutgoingData*	handleUrlEncodedForm(const HTTPRequest& req, const size_t maxBodySize,
								map_str *toForm = NULL, int *respStatus = NULL,
								std::string toFileName = "");
	static OutgoingData*	handleJsonPost(const HTTPRequest& req, const size_t maxBodySize);
	static std::string		urlDecode(const std::string& encoded);
};
