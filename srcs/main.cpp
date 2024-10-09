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

int main()
{
    Timer timer;
    timer.startTimer();
    Config config("configs/default.conf");
    config.printConfig();
    timer.printElapsedTime();
    std::cout << timer.getElapsedTimeMicroseconds() << " µs"  << std::endl;
    std::cout << "is it bigger than: 110 us? " << ((timer.isTimeElapsedUs(110) == 1) ? "true" : "false") << std::endl;
    
    return 0;
}
