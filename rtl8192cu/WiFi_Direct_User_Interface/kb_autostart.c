#include <stdio.h>
#include <fcntl.h>
#include <linux/uinput.h>
#include <string.h>

#define EV_PRESSED 1	//keyboard pressed
#define EV_RELEASED 0	//keyboard released
#define EV_REPEAT 2

struct uinput_user_dev uidev;
struct input_event ev;		// keyboard_event structure

char path[100] = "/home/pi/git/rtl8192cu/WiFi_Direct_User_Interface/pi_main";
char path2[100] = "cd /home/pi/git/rtl8192cu/WiFi_Direct_User_Interface/";
//

int shift_flag;

int uinput_fd;

void Call_key(char ch);
void Key_event(char ch);

int main(void)
{
	int i, j;
	int ret;
	int path_len;

	uinput_fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK | O_CREAT);

	if(uinput_fd < 0 )	{
		perror("OPEN ERROR!!!\n");
	}

	ret = ioctl(uinput_fd, UI_SET_EVBIT, EV_KEY);
	ret = ioctl(uinput_fd, UI_SET_EVBIT, EV_SYN);

	for(i=0; i<255; i++)	{
		ioctl(uinput_fd, UI_SET_KEYBIT, i);
	}

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
	
	sleep(25);	//It is wait for start GUI environment.

	path_len = strlen(path2);
	for(i=0; i<path_len; i++)	{
		Call_key(path2[i]);	//call key it!!!
	}
	usleep(10000);
	Key_event(KEY_ENTER);	//enter it

	usleep(10000);
	path_len = strlen(path);
	for(i=0; i<path_len; i++)	{
		Call_key(path[i]);	//call key it!!!
	}
	usleep(10000);
	Key_event(KEY_ENTER);	//enter it

	return 0;
}

void Call_key(char ch)
{
		if((ch >= 'A' && ch <= 'Z') || ch == '_')	{
			Key_event(KEY_RIGHTSHIFT);		
		}

		switch(ch)	{
			case '1':
				Key_event(KEY_1);
			break;
			case '2':
				Key_event(KEY_2);
			break;	
			case '3':
				Key_event(KEY_3);
			break;
			case '4':
				Key_event(KEY_4);
			break;
			case '5':
				Key_event(KEY_5);
			break;
			case '6':
				Key_event(KEY_6);
			break;
			case '7':
				Key_event(KEY_7);
			break;
			case '8':
				Key_event(KEY_8);
			break;
			case '9':
				Key_event(KEY_9);
			break;
			case '0':
				Key_event(KEY_0);
			break;
			case 'q':
			case 'Q':
				Key_event(KEY_Q);
			break;
			case 'w':
			case 'W':
				Key_event(KEY_W);
			break;
			case 'e':
			case 'E':
				Key_event(KEY_E);
			break;
			case 'r':
			case 'R':
				Key_event(KEY_R);
			break;
			case 't':
			case 'T':
				Key_event(KEY_T);
			break;
			case 'y':
			case 'Y':
				Key_event(KEY_Y);
			break;
			case 'u':
			case 'U':
				Key_event(KEY_U);
			break;
			case 'i':
			case 'I':
				Key_event(KEY_I);
			break;
			case 'o':
			case 'O':
				Key_event(KEY_O);
			break;
			case 'p':
			case 'P':
				Key_event(KEY_P);
			break;
			case 'a':
			case 'A':
				Key_event(KEY_A);
			break;
			case 's':
			case 'S':
				Key_event(KEY_S);
			break;
			case 'd':
			case 'D':
				Key_event(KEY_D);
			break;
			case 'f':
			case 'F':
				Key_event(KEY_F);
			break;
			case 'g':
			case 'G':
				Key_event(KEY_G);
			break;
			case 'h':
			case 'H':
				Key_event(KEY_H);
			break;
			case 'j':
			case 'J':
				Key_event(KEY_J);
			break;
			case 'k':
			case 'K':
				Key_event(KEY_K);
			break;
			case 'l':
			case 'L':
				Key_event(KEY_L);
			break;
			case 'z':
			case 'Z':
				Key_event(KEY_Z);
			break;
			case 'x':
			case 'X':
				Key_event(KEY_X);
			break;
			case 'c':
			case 'C':
				Key_event(KEY_C);
			break;
			case 'v':
			case 'V':
				Key_event(KEY_V);
			break;
			case 'b':
			case 'B':
				Key_event(KEY_B);
			break;
			case 'n':
			case 'N':
				Key_event(KEY_N);
			break;
			case 'm':
			case 'M':
				Key_event(KEY_M);
			break;
			case '/':
				Key_event(KEY_SLASH);
			break;
			case '.':
				Key_event(KEY_DOT);
			break;
			case '-':
			case '_':
				Key_event(KEY_MINUS);
			break;
			case ' ':
				Key_event(KEY_SPACE);
			break;
			default :
				printf("It is not good path!!!check path!!!\n");
				return 0;
			break;
		}	//switch!!!

		if((ch >= 'A' && ch <= 'Z') || ch == '_')
			Key_event(KEY_RIGHTSHIFT);	

}

void Key_event(char ch)
{
	int status = 2;	// 0-release, 1-press, 2-all || 2 is init

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

			usleep(10000);	//10000us = 10ms
		}

		if(status == 2 || status == 0)	{
			//Release the key
			usleep(10000);

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
	
