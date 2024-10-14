/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tviejo <tviejo@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/08 12:48:33 by tviejo            #+#    #+#             */
/*   Updated: 2024/10/08 12:48:43 by tviejo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "webserv.hpp"
#include <ostream>

std::string	handleGetResponse(const std::string& request);
std::string	handlePostResponse(const std::string& request);
std::string	handleDeleteResponse(const std::string& request);
std::string	makeResponse(uint32_t status, const std::string& statusMessage, const std::string& path, const std::string& content);
