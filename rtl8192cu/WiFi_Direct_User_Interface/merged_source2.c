//***THIS IS WIFI CONNECTION PROGRAM

/*gcc -o merged_source2 ./merged_source2.c ./p2p_api_test_linux.c -lpthread -I/usr/include/*/

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
void Mouse_move(int x, int y);

//*** KEYBOARD SECTION *************************
void Key_event(char ch);	//keyboard function

struct input_event ev;		// keyboard_event structure
struct input_event m_ev;	// ############## Mouse_event structure
struct uinput_user_dev uidev;

char ipaddr[20];

int uinput_fd;

int shift_flag;
int ctrl_flag;
int alt_flag;
//This flag is check the pressed, released - keyboard

int sys_return;
int redirect = -1;
void wifi_direct_connect(void);
char *naming_wpsinfo(int wps_info);
char *naming_role(int role);
char *naming_status(int status);
char* naming_enable(int enable);
void ui_screen(struct p2p *p);
void init_p2p(struct p2p *p);
void rename_intf(struct p2p *p);

int main(int argc, char **argv)
{

	
/************ Wifi Direct Connection *********************/

	printf("Wait for some seconds\n");
	
	
	wifi_direct_connect();	
	
	
	printf("Thread start!\n");

	pthread_t thread_num[2];
	int status;
	
	pthread_create(&thread_num[0],NULL, frameThread, NULL); //framebuffer
	pthread_create(&thread_num[1], NULL, inputThread, NULL);

	pthread_join(thread_num[0], (void **)status);
	pthread_join(thread_num[1], (void **)status);

	printf("Thread finish!\n");

	return 0;

}	//main


/***************************************/

	/* Disable P2P functionalities when exits*/
	/*
	p->enable= -1 ;
	p2p_enable(p);

	system( "rm -f ./supp_status.txt" );
	system( "rm -f ./temp.txt" );
	system( "rm -f ./scan.txt" );
	system( "rm -f ./peer.txt" );
	system( "rm -f ./status.txt" );
	system( "rm -f ./cm.txt" );*/

void wifi_direct_connect()
{
	int ipfd,sockfd, n;
	struct ifreq ifr;
	struct sockaddr_in servaddr,cliaddr;
	
	char peerifa[40] = { 0x00 };
	char scan[CMD_SZ];	
	struct p2p p2pstruct;
	struct p2p *p=NULL;

	p = &p2pstruct;	
	if( p != NULL)
	{
		memset(p, 0x00, sizeof(struct p2p));
		init_p2p(p);
	}

	strcpy(p->ifname, "wlan0" );
	
	
	p->enable=P2P_ROLE_DISABLE;
	p2p_enable(p);
	p2p_get_hostapd_conf(p); //get hostapd.conf
	//usleep(50000);  
  	rename_intf(p);
	
	ui_screen(p);

	//p->show_scan_result = 1;
	//ui_screen(p);
	p->enable = 1;
	p2p_enable(p);
	//p->show_scan_result = 1;
	//ui_screen(p);
	printf("==> p2p_enable(p)\n");

	p->thread_trigger = THREAD_NONE ;
	

	p->wps_info=3;
	p2p_wpsinfo(p);
	printf("==> p2p_wpsinfor(p)\n");

	p2p_status(p, 0);
	//printf("==> p2p_status(p)\n");

	//usleep(10000);
	
	if(p->status != P2P_STATE_GONEGO_OK)
	{
		
		p2p_set_nego(p);
		printf("...Negotiated\n");
	}
	else
	{
		p2p_role(p,0);
						
		if( p->role == P2P_ROLE_CLIENT )
		{
			printf("==>P2P_ROLE_CLIENT\n");
			p2p_client_mode(p);
		
		}
		else if( p->role == P2P_ROLE_GO )
		{
			printf("==>P2P_ROLE_GO\n");
			p2p_go_mode(p);
		}
	}
	
	system("dhclient wlan0");
	
	/********* Want to get IP address, you should be wait!*/
   /* because it takes a little bit time ****/
	printf("WIFI DIRECT SUCCESS, Wait 2sec...\n");
	sleep(2);

	
	ipfd= socket(PF_INET, SOCK_DGRAM, 0);
	ifr.ifr_addr.sa_family = PF_INET;
	strncpy(ifr.ifr_name, "wlan0", IFNAMSIZ-1);
	ioctl(ipfd, SIOCGIFADDR, &ifr);
	close(ipfd);
	

	sockfd=socket(PF_INET, SOCK_DGRAM, 0);

	
	strcpy(ipaddr, inet_ntoa(((struct sockaddr_in *) &ifr.ifr_addr)->sin_addr));

	printf(" ==> ip : %s\n",ipaddr);

}


char *naming_wpsinfo(int wps_info)
{
	switch(wps_info)
	{

		case P2P_NO_WPSINFO: return ("P2P_NO_WPSINFO");
		case P2P_GOT_WPSINFO_PEER_DISPLAY_PIN: return ("P2P_GOT_WPSINFO_PEER_DISPLAY_PIN");
		case P2P_GOT_WPSINFO_SELF_DISPLAY_PIN: return ("P2P_GOT_WPSINFO_SELF_DISPLAY_PIN");

		case P2P_GOT_WPSINFO_PBC: return ("P2P_GOT_WPSINFO_PBC");
		default: return ("UI unknown failed");
	}
}

char *naming_role(int role)
{
	switch(role)

	{
		case P2P_ROLE_DISABLE: return ("P2P_ROLE_DISABLE");
		case P2P_ROLE_DEVICE: return ("P2P_ROLE_DEVICE");

		case P2P_ROLE_CLIENT: return ("P2P_ROLE_CLIENT");
		case P2P_ROLE_GO: return ("P2P_ROLE_GO");
		default: return ("UI unknown failed");
	}

}

char *naming_status(int status)
{
	switch(status)
	{
		case P2P_STATE_NONE: return ("P2P_STATE_NONE");
		case P2P_STATE_IDLE: return ("P2P_STATE_IDLE");
		case P2P_STATE_LISTEN: return ("P2P_STATE_LISTEN");
		case P2P_STATE_SCAN: return ("P2P_STATE_SCAN");
		case P2P_STATE_FIND_PHASE_LISTEN: return ("P2P_STATE_FIND_PHASE_LISTEN");
		case P2P_STATE_FIND_PHASE_SEARCH: return ("P2P_STATE_FIND_PHASE_SEARCH");
		case P2P_STATE_TX_PROVISION_DIS_REQ: return ("P2P_STATE_TX_PROVISION_DIS_REQ");
		case P2P_STATE_RX_PROVISION_DIS_RSP: return ("P2P_STATE_RX_PROVISION_DIS_RSP");
		case P2P_STATE_RX_PROVISION_DIS_REQ: return ("P2P_STATE_RX_PROVISION_DIS_REQ");
		case P2P_STATE_GONEGO_ING: return ("P2P_STATE_GONEGO_ING");
		case P2P_STATE_GONEGO_OK: return ("P2P_STATE_GONEGO_OK"); //1
		case P2P_STATE_GONEGO_FAIL: return ("P2P_STATE_GONEGO_FAIL");
		case P2P_STATE_RECV_INVITE_REQ: return ("P2P_STATE_RECV_INVITE_REQ");
		case P2P_STATE_PROVISIONING_ING: return ("P2P_STATE_PROVISIONING_ING");
		case P2P_STATE_PROVISIONING_DONE: return ("P2P_STATE_PROVISIONING_DONE");
		default: return ("UI unknown failed");
	}
}

char* naming_enable(int enable)
{

	switch(enable)
	{
		case  P2P_ROLE_DISABLE: return ("[Disabled]");
		case  P2P_ROLE_DEVICE: return ("[Enable/Device]");
		case  P2P_ROLE_CLIENT: return ("[Enable/Client]");
		case  P2P_ROLE_GO: return ("[Enable/GO]");
		default: return ("UI unknown failed");
	}
}

void ui_screen(struct p2p *p)
{
	p->show_scan_result = 0;
}

void init_p2p(struct p2p *p)
{
	strcpy( p->ifname, "wlan0" );
	p->enable = P2P_ROLE_DISABLE;
	p->res = 1;
	p->res_go = 1;
	p->status = P2P_STATE_NONE;
	p->intent = 1;
	p->wps_info = 0;
	p->wpsing = 0;
	p->pin = 12345670;
	p->role = P2P_ROLE_DISABLE;
	p->listen_ch = 11;
	strcpy( p->peer_devaddr, "00:00:00:00:00:00" );
	p->p2p_get = 0;
	memset( p->print_line, 0x00, CMD_SZ);
	p->have_p2p_dev = 0;
	p->count_line = 0;
	strcpy( p->peer_ifaddr, "00:00:00:00:00:00" );
	memset( p->cmd, 0x00, CMD_SZ);
	p->wpa_open=0;

	p->ap_open=0;
	strcpy(p->ok_msg, "WiFi Direct handshake done" );
	strcpy(p->redo_msg, "Re-do GO handshake" );
	strcpy(p->fail_msg, "GO handshake unsuccessful" );
	strcpy(p->nego_msg, "Start P2P negotiation" );
	strcpy(p->wpa_conf, "./wpa_0_8.conf" );
	strcpy(p->wpa_path, "./wpa_supplicant" );
	strcpy(p->wpacli_path, "./wpa_cli" );
	strcpy(p->ap_conf, "./p2p_hostapd.conf" );
	strcpy(p->ap_path, "./hostapd" );
	strcpy(p->apcli_path, "./hostapd_cli" );
	strcpy(p->scan_msg, "Device haven't enable p2p functionalities" );
	
}

void rename_intf(struct p2p *p)
{
	FILE *pfin = NULL;
	FILE *pfout = NULL;
	
	pfin = fopen( p->ap_conf, "r" );
	pfout = fopen( "./p2p_hostapd_temp.conf", "w" );
	
	if ( pfin )
	{
		while( !feof( pfin ) ){
			memset(p->parse, 0x00, CMD_SZ);
			fgets(p->parse, CMD_SZ, pfin);
			
			if(strncmp(p->parse, "interface=", 10) == 0)
			{
				memset(p->parse, 0x00, CMD_SZ);
				sprintf( p->parse, "interface=%s\n", p->ifname );
				fputs( p->parse, pfout );
			}
			else
				fputs(p->parse, pfout);
		}

	}

	fclose( pfout );
	
	return;
}
/*************************************************************/

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

	inet_pton(PF_INET, ipaddr ,&servaddr.sin_addr);
	if(-1 == bind(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)))
	{
		printf("BIND ERROR\n");
		return 0;
	}
	printf("socket : %s\n", inet_ntoa(servaddr.sin_addr));
	printf("[Success] OUTPUT Connect Success\n");
	int num=0;

	int client_addr_size = sizeof(cliaddr);
	mesg[0]=0;
	recvfrom(sockfd, mesg, 1, 0, (struct sockaddr*)&cliaddr, &client_addr_size);
	

	while(1)
	{
		

		for (y = 0; y < 8; y++)	{	//384
			*(fbp + ((y) * 49152)) = y;		
			returnv = sendto(sockfd, (fbp + ((y) * 49152)), 49152, 0, (struct sockaddr *)&cliaddr, sizeof(cliaddr));
		
		}	//y

	}	//while

}	//frameThread

