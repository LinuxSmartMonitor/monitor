// NOTICE!!!
// THIS IS USE THE FB 1
// TRANSFER AT ONE LINE(NO ONE PIXEL)
//	IF YOU WANT FB0, MOVE open(fb0)...

#include <sys/types.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <usb.h>
#include <libusb.h>
#include <string.h>	
#include <unistd.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <signal.h>

#include <errno.h>
#include <linux/input.h>
#include <linux/uinput.h>


//#define IN 0x81	//NEXUS ONE

//#define IN 0x82	//TAB
//#define OUT 0x04

#define IN 0x085	//*** FOR TEST S3
#define OUT 0x004	//*** FOR TEST S3

//#define IN 0x82	//*** FOR TEST NOTE 2
//#define OUT 0x01	//*** FOR TEST NOTE 2

//#define IN 0x081	//*** FOR TEST S4
//#define OUT 0x04	//*** FOR TEST S4

//#define IN 0x082	//*** FOR TEST S5
//#define OUT 0x02	//*** FOR TEST S5


#define VID 0x04E8	//SAMSUNG
#define PID 0x6860

#define ACCESSORY_VID 0x18D1
#define ACCESSORY_PID 0x2D01
#define ACCESSORY_PID_ALT 0x2D00

//*** KEYBOARD DEFINE
#define EV_PRESSED 1	//keyboard pressed
#define EV_RELEASED 0	//keyboard released
#define EV_REPEAT 2

/*
gcc simplectrl2.c -I/usr/include/ -o simplectrl2 -lusb-1.0 -I/usr/include/ -I/usr/include/libusb-1.0 -lpthread
*/

void *mainPhase(void *);


static int init(void);
static int deInit(void);
static void error(int code);
static void status(int code);
static int setupAccessory(
const char* manufacturer,
const char* modelName,
const char* description,
const char* version,
const char* uri,
const char* serialNumber);
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

void *inputThread(void *arg)
{
	//############## Mouse Event Start 1 ####################

	int current_x_coord, current_y_coord, mouse_status;
	int before_x_coord, before_y_coord;
	
	//############## Mouse Event End 1 ####################

	//************FOR KEYBOARD ************
	int ret;

	int i;

	printf("Starting the keyboard buffer writer/reader \n");

	fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK | O_CREAT);
	
	if(fd < 0 )	{
		perror("OPEN ERROR!!!\n");
	}

	ret = ioctl(fd, UI_SET_EVBIT, EV_KEY);
	ret = ioctl(fd, UI_SET_EVBIT, EV_SYN);
	//ret = ioctl(fd, UI_SET_EVBIT, EV_REL);
	//ret = ioctl(fd, UI_SET_EVBIT, EV_ABS);
	//ret = ioctl(fd, UI_SET_EVBIT, EV_MSC);
	//ret = ioctl(fd, UI_SET_EVBIT, EV_SW);
	//ret = ioctl(fd, UI_SET_EVBIT, EV_LED);
	//ret = ioctl(fd, UI_SET_EVBIT, EV_SND);
	//ret = ioctl(fd, UI_SET_EVBIT, EV_REP);
	//ret = ioctl(fd, UI_SET_EVBIT, EV_FF);
	//ret = ioctl(fd, UI_SET_EVBIT, EV_PWR);
	//ret = ioctl(fd, UI_SET_EVBIT, EV_FF_STATUS);
	//ret = ioctl(fd, UI_SET_EVBIT, EV_MAX);
	//ret = ioctl(fd, UI_SET_EVBIT, EV_CNT);

	//############## Mouse Event Start 2 ####################
	//ret = ioctl(fd, UI_SET_EVBIT, EV_KEY);
	//ret = ioctl(fd, UI_SET_EVBIT, EV_SYN);

	ret = ioctl(fd, UI_SET_KEYBIT, BTN_MOUSE);
	ret = ioctl(fd, UI_SET_KEYBIT, BTN_LEFT);
	ret = ioctl(fd, UI_SET_KEYBIT, BTN_RIGHT);
	ret = ioctl(fd, UI_SET_EVBIT, EV_REL);
	ret = ioctl(fd, UI_SET_RELBIT, REL_X);
	ret = ioctl(fd, UI_SET_RELBIT, REL_Y);
	//############## Mouse Event End 2 ####################

	if(ret < 0)	{
		perror("Mouse ioctl error!!!\n");
	}

	if(ret < 0)	{
		perror("ret ioctl error!!!\n");
	}	

	for(i=0; i<255; i++)
	{
		ioctl(fd, UI_SET_KEYBIT, i);
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
	ret = write(fd, &uidev, sizeof(uidev) );
	if(ret < 0)	{
		perror("write error!!!\n");
	}		
	//***create the device
	ret = ioctl(fd, UI_DEV_CREATE);
	if(ret < 0)	{
		perror("UI_DEV_CREATE error!!!\n");
	}

	//############## Mouse Event Start 3 ####################

	//ret = ioctl(fd, UI_SET_EVBIT, EV_KEY);	//Key press and release
	//ret = ioctl(fd, UI_SET_EVBIT, EV_SYN);	//relative axis event - mouse

	before_x_coord = 0;
	before_y_coord = 0;
	
	Mouse_move(-100,-100);

	//############## Mouse Event End 3 ####################
	
	while(1)
	{
		Key_event(30);	//a
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
		current_x_coord = rand()%30;
		current_y_coord = rand()%30;
		
		current_x_coord = current_x_coord - before_x_coord;
		current_y_coord = current_y_coord - before_y_coord;
		mouse_status = 1;	// 1 is one click, 2 is double click
		
		if(mouse_status == 1) {
			Mouse_move(current_x_coord, current_y_coord);
			before_x_coord = current_x_coord;
			before_y_coord = current_y_coord;
			//Mouse_one_click();
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

	for(i=0; i<30; i++)	{
		//printf("1 For count : %d\n",i);
		memset(&m_ev, 0, sizeof(struct input_event));
		m_ev.type = EV_REL;
		m_ev.code = REL_X;
		m_ev.value = x;
		if(write(fd, &m_ev, sizeof(struct input_event)) < 0 )
			printf("EV_REL x Fail 1\n");

		memset(&m_ev, 0, sizeof(struct input_event));
		m_ev.type = EV_REL;
		m_ev.code = REL_Y;
		m_ev.value = y;
		if(write(fd, &m_ev, sizeof(struct input_event)) < 0 )
			printf("EV_REL y Fail 1\n");

		memset(&m_ev, 0, sizeof(struct input_event));
		m_ev.type = EV_SYN;
		m_ev.code = SYN_REPORT;
		m_ev.value = 1;
		if(write(fd, &m_ev, sizeof(struct input_event)) < 0)
			printf("EV_SYN Fail 1\n");
		
		usleep(10000);
	}//for 
}

void Mouse_one_click()
{
		printf("Enter Mouse_one_click\n");
		// one click
		memset(&m_ev, 0, sizeof(struct input_event));
		m_ev.type = EV_KEY;
		m_ev.code = BTN_LEFT;
		m_ev.value = 1;
		write(fd, &m_ev, sizeof(struct input_event));

		memset(&m_ev, 0, sizeof(struct input_event));
		m_ev.type = EV_SYN;
		m_ev.code = SYN_REPORT;
		m_ev.value = 0;
		write(fd, &m_ev, sizeof(struct input_event));

		memset(&m_ev, 0, sizeof(struct input_event));
		m_ev.type = EV_KEY;
		m_ev.code = BTN_LEFT;
		m_ev.value = 0;
		write(fd, &m_ev, sizeof(struct input_event));

		memset(&m_ev, 0, sizeof(struct input_event));
		m_ev.type = EV_SYN;
		m_ev.code = SYN_REPORT;
		m_ev.value = 0;

}

//one click + one click = double click
void Mouse_double_click()
{
		printf("Enter Mouse_double_click\n");
		// one click
		memset(&m_ev, 0, sizeof(struct input_event));
		m_ev.type = EV_KEY;
		m_ev.code = BTN_LEFT;
		m_ev.value = 1;
		write(fd, &m_ev, sizeof(struct input_event));

		memset(&m_ev, 0, sizeof(struct input_event));
		m_ev.type = EV_SYN;
		m_ev.code = SYN_REPORT;
		m_ev.value = 0;
		write(fd, &m_ev, sizeof(struct input_event));

		memset(&m_ev, 0, sizeof(struct input_event));
		m_ev.type = EV_KEY;
		m_ev.code = BTN_LEFT;
		m_ev.value = 0;
		write(fd, &m_ev, sizeof(struct input_event));

		memset(&m_ev, 0, sizeof(struct input_event));
		m_ev.type = EV_SYN;
		m_ev.code = SYN_REPORT;
		m_ev.value = 0;
		write(fd, &m_ev, sizeof(struct input_event));

		// one click
		memset(&m_ev, 0, sizeof(struct input_event));
		m_ev.type = EV_KEY;
		m_ev.code = BTN_LEFT;
		m_ev.value = 1;
		write(fd, &m_ev, sizeof(struct input_event));

		memset(&m_ev, 0, sizeof(struct input_event));
		m_ev.type = EV_SYN;
		m_ev.code = SYN_REPORT;
		m_ev.value = 0;
		write(fd, &m_ev, sizeof(struct input_event));

		memset(&m_ev, 0, sizeof(struct input_event));
		m_ev.type = EV_KEY;
		m_ev.code = BTN_LEFT;
		m_ev.value = 0;
		write(fd, &m_ev, sizeof(struct input_event));

		memset(&m_ev, 0, sizeof(struct input_event));
		m_ev.type = EV_SYN;
		m_ev.code = SYN_REPORT;
		m_ev.value = 0;
		write(fd, &m_ev, sizeof(struct input_event));
}



//############## Mouse Event End 5  ####################


void Key_event(char ch)
{
		//Press a key(stuff the keyboard with a keypress)
		ev.type = EV_KEY;
		ev.value = EV_PRESSED;
		ev.code = ch;
		write(fd, &ev, sizeof(struct input_event) );

		usleep(100000);	//100000us = 100ms = 0.1s delay!!!NO DELETE
		//Release the key
		ev.value = EV_RELEASED;
		ev.code = ch;
		write(fd, &ev, sizeof(struct input_event) );
}

void Key_shift(char ch)
{
		//Shift key
		ev.type = EV_KEY;
		ev.value = EV_PRESSED;
		ev.code = KEY_LEFTSHIFT;	//KEY_RIGHTSHIFT -> this is shift key
		write(fd, &ev, sizeof(struct input_event) );

		//Press a key(stuff the keyboard with a keypress)
		ev.type = EV_KEY;
		ev.value = EV_PRESSED;
		ev.code = ch;
		write(fd, &ev, sizeof(struct input_event) );

		usleep(100000);	//100000us = 100ms = 0.1s delay!!!NO DELETE
		//Release the key
		ev.value = EV_RELEASED;
		ev.code = ch;
		write(fd, &ev, sizeof(struct input_event) );

		//Release the shift
		ev.value = EV_RELEASED;
		ev.code = KEY_LEFTSHIFT;
		write(fd, &ev, sizeof(struct input_event) );	
}

int main (int argc, char *argv[]){

	pthread_t thread_num[2];
	int status;

	int a=1;

	if(init() < 0)
		return;
	//doTransfer();
	if(setupAccessory(
		"samsong",
		"dd",
		"Description is",
		"antilope",
		"url",
		"Serial") < 0){
		fprintf(stdout, "Error setting up accessory\n");
		deInit();
		return -1;
	};

	pthread_create(&thread_num[0],NULL,mainPhase,NULL);
	pthread_create(&thread_num[1], NULL, inputThread, NULL);

	pthread_join(thread_num[0], (void **)status);
	pthread_join(thread_num[1], (void **)status);

	printf("Thread finish!\n");

}

void *mainPhase(void *arg){
	
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



/***************************************/
WIDTH = vinfo.xres;	// nubby
while(1)	{
//COPY AREA
   for (y = 0; y < vinfo.yres; y++)	{	//384
		
			//*** (1) - TRANSFER, INIT
			libusb_bulk_transfer(handle,OUT, fbp + (offset * 512), WIDTH , &transferred,0);
			offset++;

			//*** (2) - TRANSFER, INIT
			libusb_bulk_transfer(handle,OUT, fbp + (offset * 512), WIDTH , &transferred,0);
			offset++;
		}	//y
		offset = 0;
	
	}	//while

}	//mainPhase

static int init(){
	libusb_init(NULL);
	if((handle = libusb_open_device_with_vid_pid(NULL, VID, PID)) == NULL){
		fprintf(stdout, "Problem acquireing handle\n");
		return -1;
	}
	libusb_claim_interface(handle, 0);
	return 0;
}

static int deInit(){
	//TODO free all transfers individually...
	//if(ctrlTransfer != NULL)
	//	libusb_free_transfer(ctrlTransfer);
	if(handle != NULL)
		libusb_release_interface (handle, 0);
	libusb_exit(NULL);
	return 0;
}

static int setupAccessory(
	const char* manufacturer,
	const char* modelName,
	const char* description,
	const char* version,
	const char* uri,
	const char* serialNumber){

	unsigned char ioBuffer[2];
	int devVersion;
	int response;
	int tries = 5;

	response = libusb_control_transfer(
		handle, //handle
		0xC0, //bmRequestType
		51, //bRequest
		0, //wValue
		0, //wIndex
		ioBuffer, //data
		2, //wLength
        0 //timeout
	);

	if(response < 0){error(response);return-1;}

	devVersion = ioBuffer[1] << 8 | ioBuffer[0];
	fprintf(stdout,"Verion Code Device: %d\n", devVersion);
	
	usleep(1000);//sometimes hangs on the next transfer :(

	response = libusb_control_transfer(handle,0x40,52,0,0,(char*)manufacturer,strlen(manufacturer),0);
	if(response < 0){error(response);return -1;}
	response = libusb_control_transfer(handle,0x40,52,0,1,(char*)modelName,strlen(modelName)+1,0);
	if(response < 0){error(response);return -1;}
	response = libusb_control_transfer(handle,0x40,52,0,2,(char*)description,strlen(description)+1,0);
	if(response < 0){error(response);return -1;}
	response = libusb_control_transfer(handle,0x40,52,0,3,(char*)version,strlen(version)+1,0);
	if(response < 0){error(response);return -1;}
	response = libusb_control_transfer(handle,0x40,52,0,4,(char*)uri,strlen(uri)+1,0);
	if(response < 0){error(response);return -1;}
	response = libusb_control_transfer(handle,0x40,52,0,5,(char*)serialNumber,strlen(serialNumber)+1,0);
	if(response < 0){error(response);return -1;}

	fprintf(stdout,"Accessory Identification sent\n", devVersion);

	response = libusb_control_transfer(handle,0x40,53,0,0,NULL,0,0);
	if(response < 0){error(response);return -1;}

	fprintf(stdout,"Attempted to put device into accessory mode\n", devVersion);

	if(handle != NULL)
		libusb_release_interface (handle, 0);


	for(;;){//attempt to connect to new PID, if that doesn't work try ACCESSORY_PID_ALT
		tries--;
		if((handle = libusb_open_device_with_vid_pid(NULL, ACCESSORY_VID, ACCESSORY_PID)) == NULL){
			if(tries < 0){
				return -1;
			}
		}else{
			break;
		}
		sleep(1);
	}
	libusb_claim_interface(handle, 0);
	fprintf(stdout, "Interface claimed, ready to transfer data\n");
	return 0;
}

static void error(int code){
	fprintf(stdout,"\n");
	switch(code){
	case LIBUSB_ERROR_IO:
		fprintf(stdout,"Error: LIBUSB_ERROR_IO\nInput/output error.\n");
		break;
	case LIBUSB_ERROR_INVALID_PARAM:
		fprintf(stdout,"Error: LIBUSB_ERROR_INVALID_PARAM\nInvalid parameter.\n");
		break;
	case LIBUSB_ERROR_ACCESS:
		fprintf(stdout,"Error: LIBUSB_ERROR_ACCESS\nAccess denied (insufficient permissions).\n");
		break;
	case LIBUSB_ERROR_NO_DEVICE:
		fprintf(stdout,"Error: LIBUSB_ERROR_NO_DEVICE\nNo such device (it may have been disconnected).\n");
		break;
	case LIBUSB_ERROR_NOT_FOUND:
		fprintf(stdout,"Error: LIBUSB_ERROR_NOT_FOUND\nEntity not found.\n");
		break;
	case LIBUSB_ERROR_BUSY:
		fprintf(stdout,"Error: LIBUSB_ERROR_BUSY\nResource busy.\n");
		break;
	case LIBUSB_ERROR_TIMEOUT:
		fprintf(stdout,"Error: LIBUSB_ERROR_TIMEOUT\nOperation timed out.\n");
		break;
	case LIBUSB_ERROR_OVERFLOW:
		fprintf(stdout,"Error: LIBUSB_ERROR_OVERFLOW\nOverflow.\n");
		break;
	case LIBUSB_ERROR_PIPE:
		fprintf(stdout,"Error: LIBUSB_ERROR_PIPE\nPipe error.\n");
		break;
	case LIBUSB_ERROR_INTERRUPTED:
		fprintf(stdout,"Error:LIBUSB_ERROR_INTERRUPTED\nSystem call interrupted (perhaps due to signal).\n");
		break;
	case LIBUSB_ERROR_NO_MEM:
		fprintf(stdout,"Error: LIBUSB_ERROR_NO_MEM\nInsufficient memory.\n");
		break;
	case LIBUSB_ERROR_NOT_SUPPORTED:
		fprintf(stdout,"Error: LIBUSB_ERROR_NOT_SUPPORTED\nOperation not supported or unimplemented on this platform.\n");
		break;
	case LIBUSB_ERROR_OTHER:
		fprintf(stdout,"Error: LIBUSB_ERROR_OTHER\nOther error.\n");
		break;
	default:
		fprintf(stdout, "Error: unkown error\n");
	}
}

static void status(int code){
	fprintf(stdout,"\n");
	switch(code){
		case LIBUSB_TRANSFER_COMPLETED:
			fprintf(stdout,"Success: LIBUSB_TRANSFER_COMPLETED\nTransfer completed.\n");
			break;
		case LIBUSB_TRANSFER_ERROR:
			fprintf(stdout,"Error: LIBUSB_TRANSFER_ERROR\nTransfer failed.\n");
			break;
		case LIBUSB_TRANSFER_TIMED_OUT:
			fprintf(stdout,"Error: LIBUSB_TRANSFER_TIMED_OUT\nTransfer timed out.\n");
			break;
		case LIBUSB_TRANSFER_CANCELLED:
			fprintf(stdout,"Error: LIBUSB_TRANSFER_CANCELLED\nTransfer was cancelled.\n");
			break;
		case LIBUSB_TRANSFER_STALL:
			fprintf(stdout,"Error: LIBUSB_TRANSFER_STALL\nFor bulk/interrupt endpoints: halt condition detected (endpoint stalled).\nFor control endpoints: control request not supported.\n");
			break;
		case LIBUSB_TRANSFER_NO_DEVICE:
			fprintf(stdout,"Error: LIBUSB_TRANSFER_NO_DEVICE\nDevice was disconnected.\n");
			break;
		case LIBUSB_TRANSFER_OVERFLOW:
			fprintf(stdout,"Error: LIBUSB_TRANSFER_OVERFLOW\nDevice sent more data than requested.\n");
			break;
		default:
			fprintf(stdout,"Error: unknown error\nTry again(?)\n");
			break;
	}
}
