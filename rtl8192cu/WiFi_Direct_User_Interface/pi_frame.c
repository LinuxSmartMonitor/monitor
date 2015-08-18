#include "pi_frame.h"

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

