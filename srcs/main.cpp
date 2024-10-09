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

int main(int ac, char **av, char **env)
{
	(void)env;
	if (ac == 2)
	{
		try 
		{
			Config config(av[1]);
//			config.printConfig();
		}
		catch (const std::exception &e)
		{
			return EXIT_FAILURE;
		}
	}
	else
		return EXIT_FAILURE;
	
    return EXIT_SUCCESS;
}
