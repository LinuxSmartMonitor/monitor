#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>	// open / close
#include <fcntl.h>	// O_RDWR
#include <errno.h>
#include <linux/input.h>
#include <linux/uinput.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/fb.h>
#include <time.h>
#include <sys/ioctl.h>	//ioctl

#define EV_PRESSED 1	//keyboard pressed
#define EV_RELEASED 0	//keyboard released
#define EV_REPEAT 2

struct input_event ev;	//keyboard_event structure
struct input_event ev1[2];
static struct input_dev * _default_input_dev;                                     
struct fb_var_screeninfo vscrinfo;
struct uinput_user_dev uidev;
int fd;
int fbfd;
int ret;	//ret
static struct input_dev * _default_input_dev;

void Mouse_one_click()
{
		printf("Enter Mouse_one_click\n");
		// one click
		memset(&ev, 0, sizeof(struct input_event));
		ev.type = EV_KEY;
		ev.code = BTN_LEFT;
		ev.value = 1;
		write(fd, &ev, sizeof(struct input_event));

		memset(&ev, 0, sizeof(struct input_event));
		ev.type = EV_SYN;
		ev.code = SYN_REPORT;
		ev.value = 0;
		write(fd, &ev, sizeof(struct input_event));

		memset(&ev, 0, sizeof(struct input_event));
		ev.type = EV_KEY;
		ev.code = BTN_LEFT;
		ev.value = 0;
		write(fd, &ev, sizeof(struct input_event));

		memset(&ev, 0, sizeof(struct input_event));
		ev.type = EV_SYN;
		ev.code = SYN_REPORT;
		ev.value = 0;
		write(fd, &ev, sizeof(struct input_event));

}

//one click + one click = double click
void Mouse_double_click()
{
		printf("Enter Mouse_double_click\n");
		// one click
		memset(&ev, 0, sizeof(struct input_event));
		ev.type = EV_KEY;
		ev.code = BTN_LEFT;
		ev.value = 1;
		write(fd, &ev, sizeof(struct input_event));

		memset(&ev, 0, sizeof(struct input_event));
		ev.type = EV_SYN;
		ev.code = SYN_REPORT;
		ev.value = 0;
		write(fd, &ev, sizeof(struct input_event));

		memset(&ev, 0, sizeof(struct input_event));
		ev.type = EV_KEY;
		ev.code = BTN_LEFT;
		ev.value = 0;
		write(fd, &ev, sizeof(struct input_event));

		memset(&ev, 0, sizeof(struct input_event));
		ev.type = EV_SYN;
		ev.code = SYN_REPORT;
		ev.value = 0;
		write(fd, &ev, sizeof(struct input_event));

		// one click
		memset(&ev, 0, sizeof(struct input_event));
		ev.type = EV_KEY;
		ev.code = BTN_LEFT;
		ev.value = 1;
		write(fd, &ev, sizeof(struct input_event));

		memset(&ev, 0, sizeof(struct input_event));
		ev.type = EV_SYN;
		ev.code = SYN_REPORT;
		ev.value = 0;
		write(fd, &ev, sizeof(struct input_event));

		memset(&ev, 0, sizeof(struct input_event));
		ev.type = EV_KEY;
		ev.code = BTN_LEFT;
		ev.value = 0;
		write(fd, &ev, sizeof(struct input_event));

		memset(&ev, 0, sizeof(struct input_event));
		ev.type = EV_SYN;
		ev.code = SYN_REPORT;
		ev.value = 0;
		write(fd, &ev, sizeof(struct input_event));

}


void Mouse_move(int x, int y)
{
	printf("Enter Mouse_event\n");
	int i;

	
	for(i=0; i<40; i++)	{
		//printf("1 For count : %d\n",i);
		memset(&ev, 0, sizeof(struct input_event));
		ev.type = EV_REL;
		ev.code = REL_X;
		ev.value = x;
		if(write(fd, &ev, sizeof(struct input_event)) < 0 )
			printf("EV_REL Y Fail\n");

		memset(&ev, 0, sizeof(struct input_event));
		ev.type = EV_REL;
		ev.code = REL_Y;
		ev.value = y;
		if(write(fd, &ev, sizeof(struct input_event)) < 0 )
			printf("EV_REL X Fail\n");

		memset(&ev, 0, sizeof(struct input_event));
		ev.type = EV_SYN;
		ev.code = SYN_REPORT;
		ev.value = 1;
		if(write(fd, &ev, sizeof(struct input_event)) < 0)
			printf("EV_SYN Fail");

		usleep(10000);
	}//for 
}

int main(void)
{

	int i, j;
	int current_x_coord, current_y_coord, mouse_status;
	int before_x_coord, before_y_coord;

	printf("Starting the keyboard buffer writer/reader \n");
	
	fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK | O_CREAT);
	fbfd = open("/dev/fb0", O_RDWR);
	//width : 1824, height : 984

	if(fd < 0 | fbfd < 0)	{
		perror("OPEN ERROR!!!\n");
	}

	ret = ioctl(fd, UI_SET_EVBIT, EV_KEY);	//Key press and release
	ret = ioctl(fd, UI_SET_EVBIT, EV_SYN);	//relative axis event - mouse
	ret = ioctl(fbfd, FBIOGET_VSCREENINFO, &vscrinfo);

	ret = ioctl(fd, UI_SET_KEYBIT, BTN_MOUSE);
	ret = ioctl(fd, UI_SET_KEYBIT, BTN_LEFT);
	ret = ioctl(fd, UI_SET_KEYBIT, BTN_RIGHT);

	ret = ioctl(fd, UI_SET_EVBIT, EV_REL);
	ret = ioctl(fd, UI_SET_RELBIT, REL_X);
	ret = ioctl(fd, UI_SET_RELBIT, REL_Y);

	if(ret < 0)	{
		perror("ret ioctl error!!!\n");
	}	

	
	printf("x : %d , y : %d\n", vscrinfo.xres, vscrinfo.yres);
	//_on_create_input_dev(&_default_input_dev);	
	memset(&uidev, 0, sizeof(struct uinput_user_dev) );	//uinput device memset
	memset(&ev, 0, sizeof(struct input_event) );

	//fill the device descriptors - *** keyboard
	snprintf(uidev.name, UINPUT_MAX_NAME_SIZE, "uinput-sample");	//input device name
	
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
		
	before_x_coord = 0;
	before_y_coord = 0;
	
	// Receive x,y coordinate from Android 
//	currnt_x_coord = 5;
//	currnt_y_coord = 5;
	Mouse_move(-100,-100);

	for(i=0; i<10; i++ ) {
		// Receive x,y coordinate from Android 
		current_x_coord = i*2;
		current_y_coord = 0;

		current_x_coord = current_x_coord - before_x_coord;
		current_y_coord = current_y_coord - before_y_coord;
		mouse_status = 1;	// 1 is one click, 2 is double click
	
		

		if(mouse_status == 1) {
			Mouse_move(current_x_coord, current_y_coord);
			before_x_coord = current_x_coord;
			before_y_coord = current_y_coord;
			Mouse_one_click();
		}else if(mouse_status == 2) {
			Mouse_move(current_x_coord, current_y_coord);
			before_x_coord = current_x_coord;
			before_y_coord = current_y_coord;
			Mouse_double_click();
		}else {
			printf("Nothing\n");
		}		
		
	}

	//}

	return 0;
}
