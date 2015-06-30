#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h> 			// memset
#include <linux/input.h>
#include <linux/uinput.h>		// virtual device driver 

#define EV_PRESSED 1
#define EV_RELEASED 0	

struct input_event ev;
struct uinput_user_dev uidev;
int fd, ret;

int main() {

	int i=0;

	fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK | O_CREAT);

	if(fd < 0)	
	{
		perror("uinput can not open.\n");
		return -1;
	}

	
	/* 
		'ioctl function' hardware control
		'EV_KEY' key press and release event
		'EV_SYN' event synchronize between kernel driver and application
	*/

	ret = ioctl(fd, UI_SET_EVBIT, EV_KEY);	
	ret = ioctl(fd, UI_SET_EVBIT, EV_SYN);


	// key setting (defined input event)
	for(i=0; i<255; i++)
	{
		ioctl(fd, UI_SET_KEYBIT, i);
	}

	memset(&uidev, 0, sizeof(struct uinput_user_dev) );
	memset(&ev, 0, sizeof(struct input_event) );	

	//fill the device descriptors (uinput device)
	snprintf(uidev.name, UINPUT_MAX_NAME_SIZE, "uinput-sample");
	uidev.id.bustype = BUS_USB;
	uidev.id.vendor = 0x1234;
	uidev.id.product = 0xfedc;
	uidev.id.version = 1;

	ret = write(fd, &uidev, sizeof(uidev) );
	ret = ioctl(fd, UI_DEV_CREATE);

	if ( ret < 0 )
	{
		perror("uinput can not event.\n");
	}

	return 0;
}

