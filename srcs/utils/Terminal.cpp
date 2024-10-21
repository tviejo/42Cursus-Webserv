/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Terminal.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ade-sarr <ade-sarr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/18 13:39:10 by ade-sarr          #+#    #+#             */
/*   Updated: 2024/10/18 15:02:13 by ade-sarr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Terminal.hpp"
#include <fcntl.h>

struct termios Terminal::oldTermio, Terminal::newTermio;

void Terminal::initTermios(bool echo)
{
	fcntl(0, F_SETFL, O_NONBLOCK);
	tcgetattr(0, &oldTermio);
	newTermio = oldTermio;
	newTermio.c_lflag &= ~ICANON;	// disable buffered i/o
	if (echo)
		newTermio.c_lflag |= ECHO;	// enable echo
	else
		newTermio.c_lflag &= ~ECHO;	// disable echo
	tcsetattr(0, TCSANOW, &newTermio);
}

void Terminal::resetTermios()
{
	tcsetattr(0, TCSANOW, &oldTermio);
}

/* Non blocking, read 1 character from stdin
 * return -1 if no character available
*/
int Terminal::getch(bool echo)
{
	int ch, c2;
	initTermios(echo);
	ch = getchar();
	if (ch == 27) {  // Extended char : escape sequence ?
		if ((c2 = getchar()) != -1)
			ch = (27 << 16) + c2;
	}
	resetTermios();
	return ch;
}

/* Blocking getch() but no <return> needed
*/
int Terminal::getchWait(bool echo)
{
	int ch;
	do ch = getch(echo); while (ch == -1);
	return ch;
}

void Terminal::enableEcho()
{
	struct termios term;
	tcgetattr(0, &term);
	term.c_lflag |= ECHO;
	tcsetattr(0, TCSANOW, &term);
}

void Terminal::disableEcho()
{
	struct termios term;
	tcgetattr(0, &term);
	term.c_lflag &= ~ECHO;
	tcsetattr(0, TCSANOW, &term);
}

void Terminal::enableSignals()
{
	struct termios term;
	tcgetattr(0, &term);
	term.c_lflag |= ISIG;
	tcsetattr(0, TCSANOW, &term);
}

void Terminal::disableSignals()
{
	struct termios term;
	tcgetattr(0, &term);
	term.c_lflag &= ~ISIG;
	tcsetattr(0, TCSANOW, &term);
}
