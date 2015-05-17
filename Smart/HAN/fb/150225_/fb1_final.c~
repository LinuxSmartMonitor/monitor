// fb0 -> fb1 Transfer Source Code
 #include <stdlib.h>
 #include <unistd.h>
 #include <stdio.h>
 #include <fcntl.h>
 #include <linux/fb.h>
 #include <sys/mman.h>
 #include <sys/ioctl.h>

 int main()
 {
     int fbfd = 0;
	   int fbfd1 = 0;

     struct fb_var_screeninfo vinfo;
	struct fb_var_screeninfo vinfo1;

     struct fb_fix_screeninfo finfo;
	struct fb_fix_screeninfo finfo1;

     long int screensize = 0;
	long int screensize1 = 0;

     char *fbp = 0;
	char *fbp1 = 0;

     int x = 0, y = 0;
	int x1 = 0, y1 = 0;

     long int location = 0;
	long int location1 = 0;

     // Open the file for reading and writing
     fbfd = open("/dev/fb0", O_RDWR);
	fbfd1 = open("/dev/fb1", O_RDWR);

     if (fbfd == -1) {
         perror("Error: cannot open framebuffer device");
         exit(1);
    	}
	  if (fbfd1 == -1) {
         perror("Error: cannot open framebuffer device__1");
         exit(1);
    	}

     printf("The framebuffer device was opened successfully.\n");

     // Get fixed screen information
     if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo) == -1) {
         perror("Error reading fixed information");
         exit(2);
     }
     if (ioctl(fbfd1, FBIOGET_FSCREENINFO, &finfo1) == -1) {
         perror("Error reading fixed information__1");
         exit(2);
     }

     // Get variable screen information
     if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo) == -1) {
         perror("Error reading variable information");
         exit(3);
     }
	     if (ioctl(fbfd1, FBIOGET_VSCREENINFO, &vinfo1) == -1) {
         perror("Error reading variable information__1");
         exit(3);
     }

     printf("%dx%d, %dbpp\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel);
	     printf("%dx%d, %dbpp\n", vinfo1.xres, vinfo1.yres, vinfo1.bits_per_pixel);	
	
     // Figure out the size of the screen in bytes
     screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;
	 screensize1 = vinfo1.xres * vinfo1.yres * vinfo1.bits_per_pixel / 8;

     // Map the device to memory
     fbp = (char *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED,
                        fbfd, 0);
	  fbp1 = (char *)mmap(0, screensize1, PROT_READ | PROT_WRITE, MAP_SHARED,
                        fbfd1, 0);
     if ((int)fbp == -1) {
         perror("Error: failed to map framebuffer device to memory");
         exit(4);
     	}
     if ((int)fbp1 == -1) {
         perror("Error: failed to map framebuffer device to memory__1");
         exit(4);
     	}
     printf("The framebuffer device was mapped to memory successfully.\n");

   for (y = 0; y <vinfo.yres; y++)	{
         for (x = 0; x < vinfo.xres; x++) {

             location = (x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) +
                        (y+vinfo.yoffset) * finfo.line_length;
	         location1 = (x+vinfo1.xoffset) * (vinfo1.bits_per_pixel/8) +
                        (y+vinfo1.yoffset) * finfo1.line_length;


					//*((unsigned short int*)(fbp + location)) = 63488;	//blue - in rp_display
					//*((unsigned short int*)(fbp + location)) = 31;	//red
					//*((unsigned short int*)(fbp + location)) = 63 << 5;	//red
					//ex - write :
					//*((unsigned short int*)(fbp + location)) = value;
					//ex - read :
					// short var = *((unsigned short int*)(fbp + location));
					*((unsigned short int*)(fbp1 + location1)) = *((unsigned short int*)(fbp + location));	//red
					//*((unsigned short int*)(fbp1 + location1)) = 31;	//red


     }	//for x
	}	//for y

     // Figure out where in memory to put
	  //*** if you want to continuously get buffer, insert while(1)
     munmap(fbp, screensize);
	   munmap(fbp1, screensize1);

     close(fbfd);
	   close(fbfd1);

     return 0;
 }

