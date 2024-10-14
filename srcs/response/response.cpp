/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tviejo <tviejo@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/08 12:48:44 by tviejo            #+#    #+#             */
/*   Updated: 2024/10/08 12:48:58 by tviejo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "response.hpp"

std::string	makeResponse(uint32_t status, const std::string& statusMessage, 
						 const std::string& path, const std::string& content)
{
	std::ostringstream	response;

	response << "HTTP/1.1" << status << " " << statusMessage << "\r\n";
	response << "Content-type: " << path << "\r\n"; 
	response << "Content-length: " << content.length() << "\r\n"; 
	response << "\r\n";
	response << content;
	return response.str();
}

std::string	handleGetResponse(const std::string& request)
{
	std::string fileContent; // =read file from uri
	(void)request;
	if (fileContent.empty() == true)
		return makeResponse(404, "Not Found", "text/plain", "404 Not Found");
	else
		return makeResponse(200, "OK", "test/html", fileContent);	
}

std::string	handlePostResponse(const std::string& request)
{
	(void)request;
	return ("dummy");
}

std::string	handleDeleteResponse(const std::string& request)
{
	(void)request;
	return ("dummy");
}

