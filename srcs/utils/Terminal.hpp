/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Terminal.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ade-sarr <ade-sarr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/18 13:39:24 by ade-sarr          #+#    #+#             */
/*   Updated: 2024/10/18 14:58:34 by ade-sarr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <termios.h>
#include <stdio.h>

class Terminal
{
	private:
		static struct termios oldTermio, newTermio;

		static void	initTermios(bool echo);
		static void	resetTermios();
	public:
		static int	getch(bool echo = false);
		static int	getchWait(bool echo = false);
		static void	enableEcho();
		static void	disableEcho();
		static void	enableSignals();
		static void	disableSignals();
};
