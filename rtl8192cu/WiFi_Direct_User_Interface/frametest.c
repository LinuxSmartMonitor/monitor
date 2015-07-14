
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

int main()
{

	

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



	int ipfd,sockfd, n;
	struct ifreq ifr;
	struct sockaddr_in servaddr,cliaddr;
	socklen_t len;
	char mesg[1000];
	char ipaddr[15];
	
	int returnv;

	ipfd= socket(AF_INET, SOCK_DGRAM, 0);
	ifr.ifr_addr.sa_family = AF_INET;
	strncpy(ifr.ifr_name, "wlan0", IFNAMSIZ-1);
	ioctl(ipfd, SIOCGIFADDR, &ifr);
	close(ipfd);
	


sockfd=socket(AF_INET, SOCK_DGRAM, 0);
	if(-1 == sockfd)
	{
		printf("SOCKET FAILED\n");
		return 0;
	}
	bzero(&servaddr, sizeof(servaddr));	
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(3490);

	inet_pton(AF_INET, inet_ntoa(((struct sockaddr_in *) &ifr.ifr_addr)->sin_addr) ,&servaddr.sin_addr);
	
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
	while(1)
{
num++;

	for (y = 0; y < 8; y++)	{	//384
	
//returnv = sendto(sockfd, mesg, 1024, 0, (struct sockaddr *)&cliaddr, len);
returnv = sendto(sockfd, (fbp + ((offset) * 49152)), 49152, 0, (struct sockaddr *)&cliaddr, sizeof(cliaddr));

if(returnv==-1)
{
	printf("ERROR\n");
	return 0;
}
printf("%d th, off : %d, Sent %d bytes.\n",num,offset, returnv);
offset++;
		}	//y
		offset = 0;
}

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


	

	return 0;
}
