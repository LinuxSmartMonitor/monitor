#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/input.h>
#include <linux/uinput.h>

#define EV_PRESSED 1	//keyboard pressed
#define EV_RELEASED 0	//keyboard released
#define EV_REPEAT 2

void Key_event(char ch);	//keyboard function
void Key_shift(char ch);	//keyboard shift

struct input_event ev;	//keyboard_event structure
struct uinput_user_dev uidev;
int fd;
int ret;	//ret

int main(void)
{

	int i, j;

	printf("Starting the keyboard buffer writer/reader \n");
	
	fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK | O_CREAT);


	if(fd < 0)	{
		perror("OPEN ERROR!!!\n");
	}

	ret = ioctl(fd, UI_SET_EVBIT, EV_KEY);
	ret = ioctl(fd, UI_SET_EVBIT, EV_SYN);
	//ret = ioctl(fd, UI_SET_EVBIT, EV_REL);
	//ret = ioctl(fd, UI_SET_EVBIT, EV_ABS);
	//ret = ioctl(fd, UI_SET_EVBIT, EV_MSC);
	//ret = ioctl(fd, UI_SET_EVBIT, EV_SW);
	//ret = ioctl(fd, UI_SET_EVBIT, EV_LED);
	//ret = ioctl(fd, UI_SET_EVBIT, EV_SND);
	//ret = ioctl(fd, UI_SET_EVBIT, EV_REP);
	//ret = ioctl(fd, UI_SET_EVBIT, EV_FF);
	//ret = ioctl(fd, UI_SET_EVBIT, EV_PWR);
	//ret = ioctl(fd, UI_SET_EVBIT, EV_FF_STATUS);
	//ret = ioctl(fd, UI_SET_EVBIT, EV_MAX);
	//ret = ioctl(fd, UI_SET_EVBIT, EV_CNT);
		
	if(ret < 0)	{
		perror("ret ioctl error!!!\n");
	}	

	for(i=0; i<255; i++)
	{
		ioctl(fd, UI_SET_KEYBIT, i);
	}

	memset(&uidev, 0, sizeof(struct uinput_user_dev) );
	memset(&ev, 0, sizeof(struct input_event) );	

	//fill the device descriptors
	snprintf(uidev.name, UINPUT_MAX_NAME_SIZE, "uinput-sample");
	uidev.id.bustype = BUS_USB;
	uidev.id.vendor = 0x1234;
	uidev.id.product = 0xfedc;
	uidev.id.version = 1;


	//***write the uinput file descriptor
	ret = write(fd, &uidev, sizeof(uidev) );
	if(ret < 0)	{
		perror("write error!!!\n");
	}		
	//***create the device
	ret = ioctl(fd, UI_DEV_CREATE);
	if(ret < 0)	{
		perror("UI_DEV_CREATE error!!!\n");
	}		

	Key_event(30);	//a
	Key_event(31);	//s
	Key_event(32);	//d
	Key_event(33);	//f

	Key_shift(30);	//A
	Key_shift(31);	//S
	Key_shift(32);	//D
	Key_shift(33);	//F

	Key_event(122);	//KEY_HANGEUL
	Key_shift(16);	//a
	Key_shift(17);	//s
	Key_shift(18);	//d
	Key_shift(19);	//f

	return 0;
}

void Key_event(char ch)
{
		//Press a key(stuff the keyboard with a keypress)
		ev.type = EV_KEY;
		ev.value = EV_PRESSED;
		ev.code = ch;
		write(fd, &ev, sizeof(struct input_event) );

		usleep(100000);	//100000us = 100ms = 0.1s delay!!!NO DELETE
		//Release the key
		ev.value = EV_RELEASED;
		ev.code = ch;
		write(fd, &ev, sizeof(struct input_event) );
}

void Key_shift(char ch)
{
		//Shift key
		ev.type = EV_KEY;
		ev.value = EV_PRESSED;
		ev.code = KEY_LEFTSHIFT;	//KEY_RIGHTSHIFT -> this is shift key
		write(fd, &ev, sizeof(struct input_event) );

		//Press a key(stuff the keyboard with a keypress)
		ev.type = EV_KEY;
		ev.value = EV_PRESSED;
		ev.code = ch;
		write(fd, &ev, sizeof(struct input_event) );

		usleep(100000);	//100000us = 100ms = 0.1s delay!!!NO DELETE
		//Release the key
		ev.value = EV_RELEASED;
		ev.code = ch;
		write(fd, &ev, sizeof(struct input_event) );

		usleep(100000);	//100000us = 100ms = 0.1s delay!!!NO DELETE
		//Release the shift
		ev.value = EV_RELEASED;
		ev.code = KEY_LEFTSHIFT;
		write(fd, &ev, sizeof(struct input_event) );	
}

/*
	//Read the key back from the keyboard buffer
	int fd1 = 0;
	if( (fd1 = open(device, O_RDONLY) ) > 0)	//It's important to open a new file descriptor here or the program will block
	{
		struct input_event event;
		unsigned int scan_code = 0;
		
		if(event.type != EV_KEY)
			return 0;
		
		if(event.value == EV_RELEASED)
		{
			scan_code = event.code;
			printf("read back scan_code is : %d\n", scan_code);
		}
		close(fd1);
	}
*/
