#pragma once

#include<iostream>
#include <termios.h>
#include <stdio.h>

//Prototypes
//char inputReader();
//void updateWorld();
//void renderWorld();
/*void initTermios(int echo);
void resetTermios(void);
char getch_(int echo);
char getch(void);
char getche(void);*/

class NonBlockingGetch
{
	private:
		static struct termios oldChars, newChars;

		static void initTermios(int echo);
		static void resetTermios(void);
		static char getch_(int echo);
	public:
		static char getch(void);
		static char getche(void);
};
