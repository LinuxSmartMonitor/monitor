//***THIS IS WIFI CONNECTION PROGRAM

/*gcc -o ./pi_main.c ./pi_p2p_api.c ./pi_input.c ./pi_frame.c -lpthread -I/usr/include/*/

#include "pi_p2p_api.h"
#include "pi_input.h"
#include "pi_frame.h"

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

char ipaddr[20];

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




