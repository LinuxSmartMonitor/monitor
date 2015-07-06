
#include "p2p_test.h"
/*gcc -o p2ptest ./p2p_ui_test_2.c ./p2p_api_test_linux.c -lpthread*/
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

	system("clear");
	printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
	printf("****************************************************************************************************\n");
	printf("*                                      P2P UI TEST v0.5                                            *\n");
	printf("****************************************************************************************************\n");
	printf("* Enable: %-89s*\n", naming_enable(p->enable));
	printf("* Intent: %2d                                                                                       *\n", p->intent);
	printf("* Status: %-89s*\n", naming_status(p->status));
	printf("* Role: %-91s*\n", naming_role(p->role));
	printf("* WPS method: %-85s*\n", naming_wpsinfo(p->wps_info));
	printf("* PIN code: %08d                                                                               *\n", p->pin);
	
	printf("* Device name: %-84s*\n", p->dev_name);
	printf("* Peer device address: %-76s*\n", p->peer_devaddr);
	printf("* Peer interface address: %-73s*\n", p->peer_ifaddr);
	
	printf("*                                                                                                  *\n");
	printf("* e) Wi-Fi Direct Enable/Disable                                                                   *\n");
	printf("* i) Intent ( The degree to be Group Owner/SoftAP )                                                *\n");
	printf("* a) Scan Wi-Fi Direct devices                                                                     *\n");
	printf("* m) Peer device address you want to test                                                          *\n");
	printf("* p) Provision discovery                                                                           *\n");
	printf("* c) Input PIN codes                                                                               *\n");
	printf("* w) WPS method                                                                                    *\n");
	printf("* n) Group owner negotiation                                                                       *\n");
	printf("* x) Start wpa_supplicant/hostapd                                                                  *\n");
	printf("* h) Set operation channel                        | t) Set SoftAP ssid                             *\n");
	printf("* r) Get Current P2P Role                         | s) Get Current P2P Status                      *\n");
	printf("* d) Set device name                              | l) Set Listen channel                          *\n");
	printf("* f) Reflash Current State                        | q) Quit                                        *\n");
	printf("****************************************************************************************************\n");
	
	
	if(p->p2p_get==0)
	{
		printf("*                                                                                                  *\n");
	}	
	else if(p->p2p_get==1)
	{
		printf("*%-98s*\n", p->print_line);
	}
	
	printf("****************************************************************************************************\n");
	
	if( ( p->show_scan_result == 1 ) && ( p->have_p2p_dev == 1 ) )
	//if( (p->have_p2p_dev == 1) && (p->enable >= P2P_ROLE_DEVICE) && ( p->wpsing == 0 ) && (p->status >= P2P_STATE_LISTEN && p->status <= P2P_STATE_FIND_PHASE_SEARCH) )
	{
			scan_result(p);
	}
	else
	{
		int i=0;
		for(i = 0; i < SCAN_POOL_NO + 1; i++ )
			printf("*                                                                                                  *\n");
	}	
	
	printf("****************************************************************************************************\n");

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


//int main()
int main(int argc, char **argv)
{
	char	peerifa[40] = { 0x00 };
	char	scan[CMD_SZ];	
	struct p2p p2pstruct;
	struct p2p *p=NULL;

	p = &p2pstruct;	
	if( p != NULL)
	{
		memset(p, 0x00, sizeof(struct p2p));
		init_p2p(p);
	}

	strcpy(p->ifname, argv[1] );
	
	/* Disable P2P functionalities at first*/
	p->enable=P2P_ROLE_DISABLE;
	p2p_enable(p);
	p2p_get_hostapd_conf(p);
	usleep(50000);  
  rename_intf(p);





				ui_screen(p);

				p->show_scan_result = 1;
				ui_screen(p);
				printf("Please insert enable mode;[0]Disable, [1]Device, [2]Client, [3]GO:");
				//scanf("%d",&p->enable);
				p->enable = 1;
				p2p_enable(p);
				p->show_scan_result = 1;
				ui_screen(p);


			p->thread_trigger = THREAD_NONE ;
			
			


				p->wps_info=3;
				p2p_wpsinfo(p);
				
				p2p_status(p, 0);
				
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















  /*
	do
	{
		ui_screen(p);

		printf("*insert cmd:");
		memset( scan, 0x00, CMD_SZ );
		scanf("%s", scan);

		if( p->thread_trigger == THREAD_NONE )		//Active mode for user interface
		{
			
			if( strncmp(scan, "e", 1) == 0 )	//Enable
			{
				p->show_scan_result = 1;
				ui_screen(p);
				printf("Please insert enable mode;[0]Disable, [1]Device, [2]Client, [3]GO:");
				scanf("%d",&p->enable);
	
				p2p_enable(p);
				p->show_scan_result = 1;
			}
#ifndef P2P_AUTO

#endif //P2P_AUTO

		}
		else if( p->thread_trigger == THREAD_DEVICE )		//Passive mode for user interface
		{
			p->thread_trigger = THREAD_NONE ;
			
			if( strncmp(scan, "b", 1) == 0 )
			{
				p->wps_info=3;
				p2p_wpsinfo(p);
				
				p2p_status(p, 0);
				
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
			}
			else if( strncmp(scan, "c", 1) == 0 )
			{
				p->wps_info=2;
				p2p_wpsinfo(p);
				
				p2p_status(p, 0);
							
				if(p->status != P2P_STATE_GONEGO_OK)
				{
					p2p_set_nego(p);
				}					
				else
				{
					p2p_role(p,0);
					p2p_ifaddr(p);
										
					if( p->role == P2P_ROLE_CLIENT )
					{
						p2p_client_mode(p);
					}
					else if( p->role == P2P_ROLE_GO )
					{
						p2p_go_mode(p);
					}
				}
			}
			else if( ('0' <= *scan ) && ( *scan <= '9') )
			{
				printf("In passive pin code\n");
				
				p->pin = atoi(scan);
				
				p->wps_info=1;
				p2p_wpsinfo(p);
				
				p2p_set_nego(p);
			}
		}
		else if( p->thread_trigger == THREAD_GO )		//Passive mode for user interface
		{
			
			p->thread_trigger = THREAD_NONE ;
			
			if( strncmp(scan, "b", 1) == 0 )
			{
				p->wps_info=3;
				p2p_wpsinfo(p);

			}
			else if( strncmp(scan, "c", 1) == 0 )
			{
				p->wps_info=2;
				p2p_wpsinfo(p);
			}
			else if( ('0' <= *scan ) && ( *scan <= '9') )
			{
				printf("In passive pin code\n");
				
				p->pin = atoi(scan);
				
				p->wps_info=1;
				p2p_wpsinfo(p);
			}
			
			p2p_go_mode(p);
			
		}
	}
	while( 1 );*/


	while(1)
{
scanf("%c",&scan);
if(strncmp(scan,"q",1)==0)
{
	break;
}
}
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
