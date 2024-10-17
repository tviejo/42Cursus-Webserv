/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ade-sarr <ade-sarr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/07 12:39:54 by tviejo            #+#    #+#             */
/*   Updated: 2024/10/17 01:49:47 by ade-sarr         ###   ########.fr       */
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
		std::cout << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
