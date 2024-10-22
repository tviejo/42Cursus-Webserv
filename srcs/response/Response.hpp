/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ade-sarr <ade-sarr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/08 12:48:33 by tviejo            #+#    #+#             */
/*   Updated: 2024/10/22 20:09:57 by ade-sarr         ###   ########.fr       */
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
	static OutgoingData *	handleGet(const t_server &, const HTTPRequest &);
	static OutgoingData *	handlePost(const t_server &, const HTTPRequest &);
	static OutgoingData *	handleDelete(const t_server &, const HTTPRequest &);
	static std::string		makeResponseHeader(uint32_t status,
										 const std::string & statusMessage,
										 const std::string & contentType,
									 	 size_t contentLength);
	static OutgoingData *	makeResponse(uint32_t status,
										 const std::string & statusMessage,
										 const std::string & contentType,
									 	 const std::string & content);
	static void				setupContentTypeMap();
	static std::string		getContentType(const std::string & uri);
};
