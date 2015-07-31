/*gcc -o frametest2 frametest2.c -I/usr/include -lpthread*/

#include "p2p_test.h"
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
#include <linux/fb.h>
#include <sys/mman.h>
#include <signal.h>
#include <pthread.h>
#include <errno.h>
#include <linux/input.h>
#include <linux/uinput.h>

//*** KEYBOARD DEFINE
#define EV_PRESSED 1	//keyboard pressed
#define EV_RELEASED 0	//keyboard released
#define EV_REPEAT 2

void *frameThread(void *arg);	
void *inputThread(void *arg);

void Mouse_one_click(void);	//mouse one click method.
void Mouse_double_click(void);
void Mouse_move(int x, int y);

//*** KEYBOARD SECTION *************************
void Key_event(char ch);	//keyboard function
void Key_shift(char ch);	//keyboard shift

struct input_event ev;		// keyboard_event structure
struct input_event m_ev;	// ############## Mouse_event structure
struct uinput_user_dev uidev;

int uinput_fd;

int main()
{
	pthread_t thread_num[2];
	int status;
	
	pthread_create(&thread_num[0],NULL, frameThread, NULL); //framebuffer
	pthread_create(&thread_num[1], NULL, inputThread, NULL);

	pthread_join(thread_num[0], (void **)status);
	pthread_join(thread_num[1], (void **)status);

	printf("Thread finish!\n");

	return 0;
}


void *inputThread(void *arg)
{
////////////////////////SOCKET
	int ipfd,sockfd, n;
	struct ifreq ifr;
	struct sockaddr_in servaddr,cliaddr;

	socklen_t len;
	char mesg[1000];

	char ipaddr[15];
	
	int returnv;

	//############## Mouse Event Start 1 ####################

	int current_x_coord, current_y_coord, mouse_status;

	int before_x_coord, before_y_coord;
	
	//############## Mouse Event End 1 ####################

	//************FOR KEYBOARD ************
	int ret;

	int i;

	printf("Starting the keyboard buffer writer/reader \n");

	uinput_fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK | O_CREAT);
	
	if(uinput_fd < 0 )	{
		perror("OPEN ERROR!!!\n");
	}

	ret = ioctl(uinput_fd, UI_SET_EVBIT, EV_KEY);
	ret = ioctl(uinput_fd, UI_SET_EVBIT, EV_SYN);
	//ret = ioctl(uinput_fd, UI_SET_EVBIT, EV_REL);
	//ret = ioctl(uinput_fd, UI_SET_EVBIT, EV_ABS);
	//ret = ioctl(uinput_fd, UI_SET_EVBIT, EV_MSC);
	//ret = ioctl(uinput_fd, UI_SET_EVBIT, EV_SW);
	//ret = ioctl(uinput_fd, UI_SET_EVBIT, EV_LED);
	//ret = ioctl(uinput_fd, UI_SET_EVBIT, EV_SND);
	//ret = ioctl(uinput_fd, UI_SET_EVBIT, EV_REP);
	//ret = ioctl(uinput_fd, UI_SET_EVBIT, EV_FF);
	//ret = ioctl(uinput_fd, UI_SET_EVBIT, EV_PWR);
	//ret = ioctl(uinput_fd, UI_SET_EVBIT, EV_FF_STATUS);
	//ret = ioctl(uinput_fd, UI_SET_EVBIT, EV_MAX);
	//ret = ioctl(uinput_fd, UI_SET_EVBIT, EV_CNT);

	//############## Mouse Event Start 2 ####################
	//ret = ioctl(uinput_fd, UI_SET_EVBIT, EV_KEY);
	//ret = ioctl(uinput_fd, UI_SET_EVBIT, EV_SYN);

	ret = ioctl(uinput_fd, UI_SET_KEYBIT, BTN_MOUSE);
	ret = ioctl(uinput_fd, UI_SET_KEYBIT, BTN_LEFT);
	ret = ioctl(uinput_fd, UI_SET_KEYBIT, BTN_RIGHT);
	ret = ioctl(uinput_fd, UI_SET_EVBIT, EV_REL);
	ret = ioctl(uinput_fd, UI_SET_RELBIT, REL_X);
	ret = ioctl(uinput_fd, UI_SET_RELBIT, REL_Y);
	//############## Mouse Event End 2 ####################

	if(ret < 0)	{
		perror("Mouse ioctl error!!!\n");
	}

	if(ret < 0)	{
		perror("ret ioctl error!!!\n");
	}	


	for(i=0; i<255; i++)
	{
		ioctl(uinput_fd, UI_SET_KEYBIT, i);
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
	ret = write(uinput_fd, &uidev, sizeof(uidev) );
	if(ret < 0)	{
		perror("write error!!!\n");
	}		
	//***create the device
	ret = ioctl(uinput_fd, UI_DEV_CREATE);
	if(ret < 0)	{
		perror("UI_DEV_CREATE error!!!\n");
	}

	//############## Mouse Event Start 3 ####################

	//ret = ioctl(uinput_fd, UI_SET_EVBIT, EV_KEY);	//Key press and release
	//ret = ioctl(uinput_fd, UI_SET_EVBIT, EV_SYN);	//relative axis event - mouse

	before_x_coord = 0;
	before_y_coord = 0;
	
	Mouse_move(-100,-100);

	//############## Mouse Event End 3 ####################

	ipfd= socket(PF_INET, SOCK_DGRAM, 0);
	ifr.ifr_addr.sa_family = PF_INET;
	strncpy(ifr.ifr_name, "wlan0", IFNAMSIZ-1);
	ioctl(ipfd, SIOCGIFADDR, &ifr);
	close(ipfd);
	

	sockfd=socket(PF_INET, SOCK_DGRAM, 0);
	if(-1 == sockfd)
	{
		printf("SOCKET FAILED\n");
		return 0;
	}
	bzero(&servaddr, sizeof(servaddr));	
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(3491);

	inet_pton(AF_INET, inet_ntoa(((struct sockaddr_in *) &ifr.ifr_addr)->sin_addr) ,&servaddr.sin_addr);

	if(-1 == bind(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)))
	{
		printf("BIND ERROR\n");

		return 0;
	}
	printf("%s\n", inet_ntoa(servaddr.sin_addr));

	printf("[Success] Connect Success\n");
	int num=0;


	int client_addr_size = sizeof(cliaddr);
	char inputtemp[3072];
	int inputdata[3];
	while(1)
	{
		//printf("inputThread\n");
		printf("recieve : %d\n",recvfrom(sockfd, inputtemp, 3072, 0, (struct sockaddr*)&cliaddr, &client_addr_size));
		inputdata[0] = *(int*)(inputtemp);
		inputdata[1] = *(int*)(inputtemp+1024);
		inputdata[2] = *(int*)(inputtemp+2048);
		printf("%d %d %d\n",inputdata[0],inputdata[1],inputdata[2]);
/*****************************          [   Protocol  ]           *****************************************************/
	/* 
	inputdata[0] ,	 inputdata[1], inputdata[2]

mouse : 		x coord, 		y coord, 	status
		keyboard : 	-1, 	data, 			-1 
	*/


		if(returnv==-1)
		{
			printf("ERROR\n");
			return 0;
		}

		//Key_event(30);	//a
		//sleep(1);

		/*&
		Key_event(31);	//s

		Key_event(32);	//d
		Key_event(33);	//f

		Key_shift(30);	//A
		Key_shift(31);	//S
		Key_shift(32);	//D
		Key_shift(33);	//F

		Key_event(122);	//KEY_HANGEUL
		Key_event(16);	//ㅂ
		Key_event(17);	//ㅈ
		Key_event(18);	//ㄷ
		Key_event(19);	//ㄱ
		Key_event(122);	//KEY_HANGEUL
		*/

		//############## Mouse Event Start 4 ####################
		// Receive x,y coordinate from Android 
		current_x_coord = inputdata[0];
		current_y_coord = inputdata[1];
		//current_x_coord = rand()%30;
		//current_y_coord = rand()%30;
		
		current_x_coord = current_x_coord - before_x_coord;
		current_y_coord = current_y_coord - before_y_coord;
		//mouse_status = 1;	// 1 is one click, 2 is double click
		mouse_status = inputdata[2];


		if(mouse_status == 1) {
			Mouse_move(current_x_coord, current_y_coord);
			before_x_coord = current_x_coord;
			before_y_coord = current_y_coord;
			Mouse_one_click();
		}else if(mouse_status == 2) {
			Mouse_move(current_x_coord, current_y_coord);
			before_x_coord = current_x_coord;
			before_y_coord = current_y_coord;
			//Mouse_double_click();
		}else {
			printf("Nothing\n");
		}

		//############## Mouse Event End 4 ####################

	}

}	//inputThread

//############## Mouse Event Start 5 ####################


void Mouse_move(int x, int y)
{
	printf("\nMouse Move (%d,%d)\n",x,y);
	int i;


	//printf("1 For count : %d\n",i);
	memset(&m_ev, 0, sizeof(struct input_event));
	m_ev.type = EV_REL;
	m_ev.code = REL_X;
	m_ev.value = x*10;
	if(write(uinput_fd, &m_ev, sizeof(struct input_event)) < 0 )
		printf("EV_REL x Fail 1\n");

	memset(&m_ev, 0, sizeof(struct input_event));
	m_ev.type = EV_REL;
	m_ev.code = REL_Y;
	m_ev.value = y*10;
	if(write(uinput_fd, &m_ev, sizeof(struct input_event)) < 0 )
		printf("EV_REL y Fail 1\n");

	memset(&m_ev, 0, sizeof(struct input_event));
	m_ev.type = EV_SYN;
	m_ev.code = SYN_REPORT;
	m_ev.value = 1;
	if(write(uinput_fd, &m_ev, sizeof(struct input_event)) < 0)
		printf("EV_SYN Fail 1\n");
		
	usleep(1000);
 
}

void Mouse_one_click()
{
		// one click

		memset(&m_ev, 0, sizeof(struct input_event));
		m_ev.type = EV_KEY;
		m_ev.code = BTN_LEFT;
		m_ev.value = 1;
		write(uinput_fd, &m_ev, sizeof(struct input_event));

		memset(&m_ev, 0, sizeof(struct input_event));
		m_ev.type = EV_SYN;
		m_ev.code = SYN_REPORT;
		m_ev.value = 0;
		write(uinput_fd, &m_ev, sizeof(struct input_event));

		memset(&m_ev, 0, sizeof(struct input_event));
		m_ev.type = EV_KEY;
		m_ev.code = BTN_LEFT;
		m_ev.value = 0;
		write(uinput_fd, &m_ev, sizeof(struct input_event));

		memset(&m_ev, 0, sizeof(struct input_event));
		m_ev.type = EV_SYN;
		m_ev.code = SYN_REPORT;
		m_ev.value = 0;
		write(uinput_fd, &m_ev, sizeof(struct input_event));
}

//one click + one click = double click
void Mouse_double_click()
{
	Mouse_one_click();
	Mouse_one_click();
}



//############## Mouse Event End 5  ####################


void Key_event(char ch)
{
		//Press a key(stuff the keyboard with a keypress)
		ev.type = EV_KEY;
		ev.value = EV_PRESSED;
		ev.code = ch;
		write(uinput_fd, &ev, sizeof(struct input_event) );

		usleep(100000);	//100000us = 100ms = 0.1s delay!!!NO DELETE
		//Release the key
		ev.value = EV_RELEASED;
		ev.code = ch;
		write(uinput_fd, &ev, sizeof(struct input_event) );
}

void Key_shift(char ch)
{
		//Shift key
		ev.type = EV_KEY;
		ev.value = EV_PRESSED;
		ev.code = KEY_LEFTSHIFT;	//KEY_RIGHTSHIFT -> this is shift key

		write(uinput_fd, &ev, sizeof(struct input_event) );

		//Press a key(stuff the keyboard with a keypress)
		ev.type = EV_KEY;
		ev.value = EV_PRESSED;
		ev.code = ch;
		write(uinput_fd, &ev, sizeof(struct input_event) );

		usleep(100000);	//100000us = 100ms = 0.1s delay!!!NO DELETE
		//Release the key
		ev.value = EV_RELEASED;
		ev.code = ch;
		write(uinput_fd, &ev, sizeof(struct input_event) );

		//Release the shift
		ev.value = EV_RELEASED;
		ev.code = KEY_LEFTSHIFT;
		write(uinput_fd, &ev, sizeof(struct input_event) );	
}


void *frameThread(void *arg){
	
	int WIDTH;	// define nuby;;;
	int offset = 0;	//framebuffer pointer offset

	int response = 0;	//usb variable
	static int transferred;

	int fbfd = 0;
     struct fb_var_screeninfo vinfo;
     struct fb_fix_screeninfo finfo;
     long int screensize = 0;
     char *fbp = 0;
     int y = 0;
	////////////////////////SOCKET
	int ipfd,sockfd, n;
	struct ifreq ifr;
	struct sockaddr_in servaddr,cliaddr;
	socklen_t len;
	char mesg[1000];
	char ipaddr[15];
	
	int returnv;

/*********************************FRAMEBUFFER*********************/

     // Open the file for reading and writing
     fbfd = open("/dev/fb1", O_RDWR);
     if (fbfd == -1) {
         perror("Error: cannot open framebuffer device");
         exit(1);
    	}
     printf("The framebuffer device was opened successfully.\n");

     // Get fixed screen information
     if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo) == -1) {
         perror("Error reading fixed information");
         exit(2);
     }

     // Get variable screen information
     if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo) == -1) {
         perror("Error reading variable information");
         exit(3);
     }

     printf("%dx%d, %dbpp\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel);

     // Figure out the size of the screen in bytes
     screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;
	//define half_x

     // Map the device to memory
     fbp = (char *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED,
                        fbfd, 0);
     if ((int)fbp == -1) {
         perror("Error: failed to map framebuffer device to memory");
         exit(4);
     	}

  	 printf("The framebuffer device was mapped to memory successfully.\n");

	ipfd= socket(PF_INET, SOCK_DGRAM, 0);
	ifr.ifr_addr.sa_family = PF_INET;
	strncpy(ifr.ifr_name, "wlan0", IFNAMSIZ-1);
	ioctl(ipfd, SIOCGIFADDR, &ifr);
	close(ipfd);

	sockfd=socket(PF_INET, SOCK_DGRAM, 0);

	if(sockfd == -1)
	{
		printf("SOCKET FAILED\n");
		return 0;
	}
	bzero(&servaddr, sizeof(servaddr));	
	servaddr.sin_family = PF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(3490);

	inet_pton(PF_INET, inet_ntoa(((struct sockaddr_in *) &ifr.ifr_addr)->sin_addr) ,&servaddr.sin_addr);
	
	if(-1 == bind(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)))
	{
		printf("BIND ERROR\n");
		return 0;
	}
	printf("%s\n", inet_ntoa(servaddr.sin_addr));
	printf("[Success] Connect Success\n");
	int num=0;

	mesg[0]='r';
	mesg[1]=0;
	int client_addr_size = sizeof(cliaddr);

	recvfrom(sockfd, mesg, 2, 0, (struct sockaddr*)&cliaddr, &client_addr_size);
	printf("transmitted\n");

	WIDTH = vinfo.xres;	// nubby

	while(1)
	{
		num++;

		for (y = 0; y < 8; y++)	{	//384
		*(fbp + ((y) * 49152)) = y;		
		returnv = sendto(sockfd, (fbp + ((y) * 49152)), 49152, 0, (struct sockaddr *)&cliaddr, sizeof(cliaddr));

		if(returnv==-1)	{
			printf("ERROR\n");
			return 0;
		}
	}	//y

	}	//while

}	//frameThread

