/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ade-sarr <ade-sarr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/07 12:39:54 by tviejo            #+#    #+#             */
/*   Updated: 2024/10/18 13:35:58 by ade-sarr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

int main(int ac, char **av)
{
	if (ac != 2) {
		std::cout << "webserv syntax: webserv <config file>" << std::endl;
		std::cout << "Press 'y' to start with 'configs/default.conf'" << std::endl;
		if (Terminal::getchWait() != 'y')
			return EXIT_FAILURE;
	}
	try	{
		std::string confFileName((ac >= 2) ? av[1] : "configs/default.conf");
		Config config(confFileName);
		config.printConfig();
		Server server(config);
		server.init();
		server.run();
	}
	catch (const std::exception &e)
	{
		std::cout << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
