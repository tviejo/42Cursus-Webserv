/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tviejo <tviejo@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/07 12:39:54 by tviejo            #+#    #+#             */
/*   Updated: 2024/10/08 16:40:09 by tviejo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

int main(int ac, char **av)
{
	if (ac != 2)
		return EXIT_FAILURE;
	try 
	{
		std::string input(av[1]);
		Config config(input);
		config.printConfig();
		Server server(config);
		server.init();
		server.run();
		//			create a socket and bind it to ip and port
		//			mark the socket for listening in and accept call
		//			close(?) the listening socket
		//			do something upon receiving message (method)
		//
	}
	catch (const std::exception &e)
	{
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
