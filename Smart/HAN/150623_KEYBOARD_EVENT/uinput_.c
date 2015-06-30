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

struct input_event ev;	//keyboard_event structure

struct uinput_user_dev uidev;
int fd;
int ret;	//ret

void Key_event(char ch);	//keyboard function
void Mouse_event(int x, int y);	//Mouse function

int main(void)
{

	int i, j;

	printf("Starting the keyboard buffer writer/reader \n");
	
	fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK | O_CREAT);


	if(fd < 0)	{
		perror("OPEN ERROR!!!\n");
	}

	ret = ioctl(fd, UI_SET_EVBIT, EV_KEY);	//Key press and release
	ret = ioctl(fd, UI_SET_EVBIT, EV_SYN);	//relative axis event - mouse

	ret = ioctl(fd, UI_SET_KEYBIT, BTN_LEFT);
	ret = ioctl(fd, UI_SET_EVBIT, EV_REL);
	ret = ioctl(fd, UI_SET_RELBIT, REL_X);
	ret = ioctl(fd, UI_SET_RELBIT, REL_Y);
	
	//ret = ioctl(fd, UI_SET_EVBIT, EV_ABS);	//absolute axis event - touchscreen event
	//ret = ioctl(fd, UI_SET_EVBIT, EV_REL);
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
		
	ret = ioctl(fd, UI_SET_ABSBIT, ABS_X);	//x
	ret = ioctl(fd, UI_SET_ABSBIT, ABS_Y);	//y

	if(ret < 0)	{
		perror("ret ioctl error!!!\n");
	}	

	for(i=0; i<255; i++)
	{
		ret = ioctl(fd, UI_SET_KEYBIT, i);	//enable keyboard keycode 0 to 255
	}

	


	memset(&uidev, 0, sizeof(struct uinput_user_dev) );	//uinput device memset
	memset(&ev, 0, sizeof(struct input_event) );

	//fill the device descriptors - *** keyboard
	snprintf(uidev.name, UINPUT_MAX_NAME_SIZE, "uinput-sample");	//input device name
	//id is device infomation
	uidev.id.bustype = BUS_USB;
	uidev.id.vendor = 0x1234;
	uidev.id.product = 0xfedc;
	uidev.id.version = 1;


	//fill the device descriptors - *** mouse
	uidev.absmin[ABS_X] = 0;
	uidev.absmax[ABS_X] = 1023;

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
		
	while(1)	
	{
		//Key_event(2);
		//Key_event(3);
		//Key_event(4);
		//Key_event(5);
		
		Mouse_event(10, 10);
		sleep(1);
	}

	return 0;
}

void Key_event(char ch)
{
		//Press a key(stuff the keyboard with a keypress)
		ev.type = EV_KEY;
		ev.value = EV_PRESSED;
		ev.code = ch;
		write(fd, &ev, sizeof(struct input_event) );

		usleep(60000);	//100000us = 100ms = 0.1s delay!!!NO DELETE
		//Release the key
		ev.value = EV_RELEASED;
		ev.code = ch;

		write(fd, &ev, sizeof(struct input_event) );
}

void Mouse_event(int x, int y)
{
	int i;
	/*
	//for(i=0; i<20; i++)	{
		memset(&ev, 0, sizeof(struct input_event));
		ev.type = EV_REL;
		ev.code = REL_X;
		ev.value = x;
		write(fd, &ev, sizeof(struct input_event));

		memset(&ev, 0, sizeof(struct input_event));
		ev.type = EV_REL;
		ev.code = REL_Y;
		ev.value = y;
		write(fd, &ev, sizeof(struct input_event));

		memset(&ev, 0, sizeof(struct input_event));
		ev.type = EV_SYN;
		ev.code = 0;
		ev.value = 0;
		write(fd, &ev, sizeof(struct input_event));

		usleep(15000);
	//}	//for i
	*/
	/*
	for(i=0; i<20; i++)	{
		memset(&ev, 0, sizeof(struct input_event));
		ev.type = EV_ABS;
		ev.code = ABS_X;
		ev.value = x;
		write(fd, &ev, sizeof(struct input_event));

		memset(&ev, 0, sizeof(struct input_event));
		ev.type = EV_ABS;
		ev.code = ABS_Y;
		ev.value = y;
		write(fd, &ev, sizeof(struct input_event));

		memset(&ev, 0, sizeof(struct input_event));
		ev.type = EV_SYN;
		ev.code = 0;
		ev.value = 0;
		write(fd, &ev, sizeof(struct input_event));

		usleep(15000);
	}
	*/
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
