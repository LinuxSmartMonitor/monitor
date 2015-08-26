//15082650826508265082650826
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
	int frsockfd,n;
	char haddr[100];
	struct sockaddr_in server_addr;
	
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

	strcpy(haddr, "192.168.49.1");

	if((frsockfd=socket(AF_INET, SOCK_DGRAM, 0))<0)
	{
		printf("SOCKET ERROR\n");
		exit(0);
	}
	bzero((char*)&server_addr, sizeof(server_addr));

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(haddr);
	server_addr.sin_port = htons(3490);
/*
	if(connect(frsockfd, (struct sockaddr *)&server_addr, sizeof(server_addr))<0)
	{
		printf("CONNECT ERROR\n");
		exit(0);
	}
	printf("CONNECT SUCCESS! \n");
		*/		
	
	while(1)
	{
		

		for (y = 0; y < 8; y++)	{	//384
			
			*(fbp + ((y) * 49152)) = y;		
			sendto(frsockfd, (fbp + ((y) * 49152)), 49152, 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
			//write(frsockfd, (fbp + ((y) * 49152)), 49152);
			
		}	//y

	}	//while

}	//frameThread

