#pragma once

#include<iostream>
#include <termios.h>
#include <stdio.h>

class NonBlockingGetch
{
	private:
		static struct termios oldChars, newChars;

		static void initTermios(int echo);
		static void resetTermios(void);
		static int getch(int echo);
	public:
		static int getch(void);
		static int getche(void);
};
