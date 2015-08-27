#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <signal.h>
#include <pthread.h>
#include <errno.h>

#include <linux/uinput.h>

//*** KEYBOARD DEFINE
#define EV_PRESSED 1	//keyboard pressed
#define EV_RELEASED 0	//keyboard released
#define EV_REPEAT 2

extern char ipaddr[20];	//it is from main ipaddr

void Mouse_one_click(void);	//mouse one click method.
void Mouse_move(int x, int y);

//*** KEYBOARD SECTION *************************
void Key_event(int ch);	//keyboard function
void Key_shift(int ch);
void Key_input(int ch);

void *inputThread(void *arg);
