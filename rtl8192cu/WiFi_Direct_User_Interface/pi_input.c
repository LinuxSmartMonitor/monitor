#include "pi_input.h"
#include "pi_onemore.h"

struct uinput_user_dev uidev;
struct input_event ev;		// keyboard_event structure
struct input_event m_ev;	// ############## Mouse_event structure

int uinput_fd;
int shift_flag;
int ctrl_flag;
int alt_flag;

//This flag is check the pressed, released - keyboard

void *inputThread(void *arg)
{
////////////////////////SOCKET
	int ipfd,sockfd, n;
	struct ifreq ifr;
	struct sockaddr_in servaddr,cliaddr;

	socklen_t len;
	char mesg[1000];

	int returnv;

	//############## Mouse Event Start 1 ####################

	int current_x_coord, current_y_coord, mouse_status;
	int before_x_coord, before_y_coord;
	int change_x_coord, change_y_coord;
	int mouse_i;
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

	before_x_coord = 0;
	before_y_coord = 0;
	for(mouse_i=0; mouse_i<50; mouse_i++) {
		Mouse_move(-512,-384);
	}

	//############## Mouse Event End 3 ####################


	sockfd=socket(PF_INET, SOCK_DGRAM, 0);

	if(-1 == sockfd)
	{
		printf("SOCKET FAILED\n");
		return 0;
	}

	bzero(&servaddr, sizeof(servaddr));	
	servaddr.sin_family = PF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(3491);

	inet_pton(PF_INET, ipaddr ,&servaddr.sin_addr);

	if(-1 == bind(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)))
	{
		printf("BIND ERROR\n");

		return 0;
	}
	printf("socket : %s\n", inet_ntoa(servaddr.sin_addr));

	printf("[Success] INPUT Connect Success\n");
	int num=0;
	

	int client_addr_size = sizeof(cliaddr);
	char inputtemp[3072];
	int inputdata[3];
	

	while(1)
	{
		recvfrom(sockfd, inputtemp, 3072, 0, (struct sockaddr*)&cliaddr, &client_addr_size);
		inputdata[0] = *(int*)(inputtemp);
		inputdata[1] = *(int*)(inputtemp+1024);
		inputdata[2] = *(int*)(inputtemp+2048);
/*****************************          [   Protocol  ]           *****************************************************/
	/* 
	inputdata[0] ,	 inputdata[1], inputdata[2]

mouse : 		x coord, 		y coord, 	status
		keyboard : 	-1, 	data, 			-1 
	*/
		if(inputdata[0] == 999 && inputdata[1] == 999 && inputdata[2] == 999){
			printf("    **** Android have exited!\n");
			onemoretime = 1;
		}
		if(returnv==-1)
		{
			printf("ERROR\n");
			return 0;
		}

		//############## Mouse Event Start 4 ####################


		if(inputdata[0] == -1)	//This is Keyboard call
		{
			Key_event(inputdata[1]);	//Key_event call
		}
		else if(inputdata[0] != -1)
		{
			// Receive x,y coordinate from Android 
			current_x_coord = inputdata[0];
			current_y_coord = inputdata[1];
		
			change_x_coord = current_x_coord - before_x_coord;
			change_y_coord = current_y_coord - before_y_coord;
			
			before_x_coord = current_x_coord;
			before_y_coord = current_y_coord;
	
			Mouse_move(change_x_coord, change_y_coord);
			Mouse_one_click();
		}	//else if(inputdata[0] != -1)

		//############## Mouse Event End 4 ####################

	}

}	//inputThread

//############## Mouse Event Start 5 ####################


void Mouse_move(int x, int y)
{
	//printf("\nMouse Move (%d,%d)\n",x,y);
	int i;

	//printf("1 For count : %d\n",i);
	memset(&m_ev, 0, sizeof(struct input_event));
	m_ev.type = EV_REL;
	m_ev.code = REL_X;
	m_ev.value = x;
	if(write(uinput_fd, &m_ev, sizeof(struct input_event)) < 0 )
		printf("EV_REL x Fail 1\n");

	memset(&m_ev, 0, sizeof(struct input_event));
	m_ev.type = EV_REL;
	m_ev.code = REL_Y;
	m_ev.value = y;
	if(write(uinput_fd, &m_ev, sizeof(struct input_event)) < 0 )
		printf("EV_REL y Fail 1\n");

	memset(&m_ev, 0, sizeof(struct input_event));
	m_ev.type = EV_SYN;
	m_ev.code = SYN_REPORT;
	m_ev.value = 1;
	if(write(uinput_fd, &m_ev, sizeof(struct input_event)) < 0)
		printf("EV_SYN Fail 1\n");
		
	usleep(10000);
 
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

//############## Mouse Event End 5  ####################


void Key_event(char ch)
{
	int status = 2;	// 0-release, 1-press, 2-all || 2 is init

		if( (ch == KEY_RIGHTCTRL || ch==KEY_LEFTCTRL) && ctrl_flag == 0)		{
			ctrl_flag = 1;
			status = 1;	//exec press		
		}
		else if( (ch == KEY_RIGHTCTRL || ch == KEY_LEFTCTRL) && ctrl_flag == 1)	{
			ctrl_flag = 0;
			status = 0;	//exec release	
		}

		if( (ch == KEY_RIGHTALT || ch == KEY_LEFTALT) && alt_flag == 0)	{
			alt_flag = 1;
			status = 1;
		}
		else if( (ch == KEY_RIGHTALT || ch == KEY_LEFTALT) && alt_flag == 1)	{
			alt_flag = 0;
			status = 0;
		}

		if( (ch == KEY_RIGHTSHIFT || ch == KEY_LEFTSHIFT) && shift_flag == 0)	{
			shift_flag = 1;
			status = 1;
		}
		else if( (ch == KEY_RIGHTSHIFT || ch == KEY_LEFTSHIFT) && shift_flag == 1)	{
			shift_flag = 0;
			status = 0;
		}


		if(status == 2 || status == 1)	{
			//press the key
			ev.type = EV_KEY;
			ev.value = EV_PRESSED;
			ev.code = ch;
			write(uinput_fd, &ev, sizeof(struct input_event) );
		
			ev.type = EV_SYN;
			ev.code = SYN_REPORT;
			ev.value = 0;
			write(uinput_fd, &ev, sizeof(struct input_event) );	

			usleep(5000);	//10000us = 10ms
		}

		if(status == 2 || status == 0)	{
			//Release the key
			usleep(5000);

			ev.type = EV_KEY;
			ev.value = EV_RELEASED;
			ev.code = ch;
			write(uinput_fd, &ev, sizeof(struct input_event) );
		
			ev.type = EV_SYN;
			ev.code = SYN_REPORT;
			ev.value = 0;
			write(uinput_fd, &ev, sizeof(struct input_event) );
		}
}
