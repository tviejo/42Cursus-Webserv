#include "NonBlockingGetch.hpp"
#include <fcntl.h>

struct termios NonBlockingGetch::oldChars, NonBlockingGetch::newChars;

void NonBlockingGetch::initTermios(int echo) //struct termios &oldChars, struct termios &newChars) 
{
  fcntl(0, F_SETFL, O_NONBLOCK);
  tcgetattr(0, &oldChars); /* grab old terminal i/o settings */
  newChars = oldChars; /* make new settings same as old settings */
  newChars.c_lflag &= ~ICANON; /* disable buffered i/o */
  newChars.c_lflag &= echo ? ECHO : ~ECHO; /* set echo mode */
  tcsetattr(0, TCSANOW, &newChars); /* use these new terminal i/o settings now */
}

/* Restore old terminal i/o settings */
void NonBlockingGetch::resetTermios(void)//struct termios &oldChars)
{
  tcsetattr(0, TCSANOW, &oldChars);
}

/* Read 1 character - echo defines echo mode */
char NonBlockingGetch::getch_(int echo) 
{
  char ch;
  initTermios(echo); //oldChars, newChars);
  ch = getchar();
  resetTermios();//oldChars);
  return ch;
}

/* Read 1 character without echo */
char NonBlockingGetch::getch(void) 
{
  return getch_(0);
}

/* Read 1 character with echo */
char NonBlockingGetch::getche(void) 
{
  return getch_(1);
}
