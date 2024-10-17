/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ade-sarr <ade-sarr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/08 12:48:33 by tviejo            #+#    #+#             */
/*   Updated: 2024/10/17 14:42:02 by ade-sarr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "webserv.hpp"
#include <ostream>

const t_route & getRouteFromUri(const t_server &, std::string uri);
std::string	handleGetResponse(const t_server &, const HTTPRequest &);
std::string	handlePostResponse(const t_server &, const HTTPRequest &);
std::string	handleDeleteResponse(const t_server &, const HTTPRequest &);
std::string	makeResponse(uint32_t status,
						 const std::string & statusMessage,
						 const std::string & contentType,
						 const std::string & content);
