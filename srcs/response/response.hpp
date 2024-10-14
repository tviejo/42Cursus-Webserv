/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ade-sarr <ade-sarr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/08 12:48:33 by tviejo            #+#    #+#             */
/*   Updated: 2024/10/14 18:10:40 by ade-sarr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "webserv.hpp"
#include <ostream>

std::string	handleGetResponse(const HTTPRequest& req);
std::string	handlePostResponse(const HTTPRequest& req);
std::string	handleDeleteResponse(const HTTPRequest& req);
std::string	makeResponse(uint32_t status, const std::string& statusMessage, const std::string& path, const std::string& content);
