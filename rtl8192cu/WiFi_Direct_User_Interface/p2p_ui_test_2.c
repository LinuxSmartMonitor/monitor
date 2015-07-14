
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

#include <errno.h>
#include <linux/input.h>
#include <linux/uinput.h>


//*** KEYBOARD DEFINE
#define EV_PRESSED 1	//keyboard pressed
#define EV_RELEASED 0	//keyboard released
#define EV_REPEAT 2

/*if you want to compile, you should use this*/
/*gcc -o p2ptest ./p2p_ui_test_2.c ./p2p_api_test_linux.c -lpthread -I/usr/include/*/
void Mouse_one_click();
void Mouse_double_click();
void Mouse_move(int x, int y);
//static
static struct libusb_device_handle* handle;


//*** KEYBOARD SECTION *************************

void *inputThread(void *arg);

void Key_event(char ch);	//keyboard function
void Key_shift(char ch);	//keyboard shift

struct input_event ev;		// keyboard_event structure
struct input_event m_ev;	// ############## Mouse_event structure
struct uinput_user_dev uidev;
int fd;

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
	
	system( "rm -rf ./p2p_hostapd.conf" );
	system( "mv ./p2p_hostapd_temp.conf ./p2p_hostapd.conf" );
	
	return;
}


int main(int argc, char **argv)
{
	char	peerifa[40] = { 0x00 };
	char	scan[CMD_SZ];	
	struct p2p p2pstruct;
	struct p2p *p=NULL;




/*********************************FRAMEBUFFER*********************/

	int fbfd = 0;
     struct fb_var_screeninfo vinfo;
     struct fb_fix_screeninfo finfo;
     long int screensize = 0;
     char *fbp = 0;
     int y = 0;
int WIDTH;	// define nuby;;;
	int offset = 0;	//framebuffer pointer offset


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
WIDTH = vinfo.xres;	// nubby







/************ Wifi Direct Connection *********************/
	p = &p2pstruct;	
	if( p != NULL)
	{
		memset(p, 0x00, sizeof(struct p2p));
		init_p2p(p);
	}

	strcpy(p->ifname, argv[1] );
	
	
	p->enable=P2P_ROLE_DISABLE;
	p2p_enable(p);
	p2p_get_hostapd_conf(p);
	usleep(50000);  
  	rename_intf(p);



	ui_screen(p);

	p->show_scan_result = 1;
	ui_screen(p);
	//scanf("%d",&p->enable);
	p->enable = 1;
	p2p_enable(p);
	p->show_scan_result = 1;
	ui_screen(p);


	p->thread_trigger = THREAD_NONE ;




	p->wps_info=3;
	p2p_wpsinfo(p);
	
	p2p_status(p, 0);

	if(p->status == P2P_STATE_LISTEN)
		{}
	else
		return 0;






	

	printf("Please Connect Your phone to WIFI DIRECT\n");


	if(p->status != P2P_STATE_GONEGO_OK)
	{
		p2p_set_nego(p);
	}
	else
	{
		p2p_role(p,0);
						
		if( p->role == P2P_ROLE_CLIENT )
		{
			p2p_client_mode(p);
		
		}
		else if( p->role == P2P_ROLE_GO )
		{
			p2p_go_mode(p);
		}
	}




	
	/********* Want to get IP address, you should be wait!*/
   /* because it takes a little bit time ****/

	printf("WIFI DIRECT SUCCESS, just after 8 seconds\n");
	printf("Wait for ...");
	int i=0;
	for(i=0; i<8; i++)
	{
		printf("%d sec\n",i);
		sleep(1);
	}
	printf("start to send data...\n");
	












/************* Opening a Socket *******************/







	int ipfd,sockfd, n;
	struct ifreq ifr;
	struct sockaddr_in servaddr, cliaddr;
	socklen_t len;
	char mesg[1000];
	char ipaddr[15];
	
	int returnv;
/*
	ipfd= socket(AF_INET, SOCK_DGRAM, 0);
	ifr.ifr_addr.sa_family = AF_INET;
	strncpy(ifr.ifr_name, "wlan0", IFNAMSIZ-1);
	ioctl(ipfd, SIOCGIFADDR, &ifr);
	close(ipfd);
	


	sockfd=socket(AF_INET, SOCK_DGRAM, 0);
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(3490);

	inet_pton(AF_INET, inet_ntoa(((struct sockaddr_in *) &ifr.ifr_addr)->sin_addr) ,&servaddr.sin_addr);
	
	bind(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));
	printf("%s\n", inet_ntoa(servaddr.sin_addr));
	printf("[Success] Connect Success\n");
	int num=0;
len = sizeof(cliaddr);
printf("len : %d\n",len);
	for(;;)
	{



len = sizeof(cliaddr);
		
	for (y = 0; y < vinfo.yres; y++)	{	//384
			returnv = sendto(sockfd, fbp + (offset * 1024), 1024, 0, (struct sockaddr *)&cliaddr, len);
printf("Sent %d bytes.\n", returnv);
			offset++;
		}	//y
		offset = 0;*/


/*
		len = sizeof(cliaddr);
		n = recvfrom(sockfd, mesg, 1000, 0, (struct sockaddr *) &cliaddr, &len);
		printf("==================\n");
		mesg[n] = 0;
		printf("%d Received the following : \n",++num);
		printf("%s\n",mesg);
		printf("==================\n");
		returnv = sendto(sockfd, mesg, n, 0, (struct sockaddr *)&cliaddr, len);
		printf("Sent %d bytes.\n", returnv);*/


	}


/***************************************/


























	/* Disable P2P functionalities when exits*/
	p->enable= -1 ;
	p2p_enable(p);

	system( "rm -f ./supp_status.txt" );
	system( "rm -f ./temp.txt" );
	system( "rm -f ./scan.txt" );
	system( "rm -f ./peer.txt" );
	system( "rm -f ./status.txt" );
	system( "rm -f ./cm.txt" );
	
	return 0;

}
