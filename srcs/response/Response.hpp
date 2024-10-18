/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ade-sarr <ade-sarr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/08 12:48:33 by tviejo            #+#    #+#             */
/*   Updated: 2024/10/18 21:30:13 by ade-sarr         ###   ########.fr       */
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
	static const t_route &	getRouteFromUri(const t_server &, std::string uri);
	static std::string		handleGet(const t_server &, const HTTPRequest &);
	static std::string		handlePost(const t_server &, const HTTPRequest &);
	static std::string		handleDelete(const t_server &, const HTTPRequest &);
	static std::string		makeResponse(uint32_t status,
										 const std::string & statusMessage,
										 const std::string & contentType,
									 	const std::string & content);
	static void				setupContentTypeMap();
	static std::string		getContentType(const std::string & uri);
};
