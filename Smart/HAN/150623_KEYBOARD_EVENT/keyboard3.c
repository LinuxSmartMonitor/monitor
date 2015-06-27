#include <stdio.h>
#include <fcntl.h>
#include <linux/input.h>

#define EV_PRESSED 1	//keyboard pressed
#define EV_RELEASED 0	//keyboard released
#define EV_REPEAT 2

void Key_event(char ch);	//keyboard function

int k_fd = 0;
char *device = "/dev/input/event1";		//This device must modify --> virtual d.d.
struct input_event k_event;	//keyboard_event structure

int main(void)
{
	printf("Starting the keyboard buffer writer/reader \n");
	
	//Write a key to the keyboard buffer
	if( (k_fd = open(device, O_RDWR) ) > 0)
	{
		Key_event(2);	//input.h -- this is 1
		Key_event(3);	//2
		Key_event(4);	//3

		close(k_fd);		//close keyboard file
	}

	return 0;
}

void Key_event(char ch)
{
		//Press a key(stuff the keyboard with a keypress)
		k_event.type = EV_KEY;
		k_event.value = EV_PRESSED;
		k_event.code = ch;
		write(k_fd, &k_event, sizeof(struct input_event) );

		//Release the key
		k_event.value = EV_RELEASED;
		k_event.code = ch;

		write(k_fd, &k_event, sizeof(struct input_event) );
}

/*
	//Read the key back from the keyboard buffer
	int fd1 = 0;
	if( (fd1 = open(device, O_RDONLY) ) > 0)	//It's important to open a new file descriptor here or the program will block
	{
		struct input_event event;
		unsigned int scan_code = 0;
		
		if(event.type != EV_KEY)
			return 0;
		
		if(event.value == EV_RELEASED)
		{
			scan_code = event.code;
			printf("read back scan_code is : %d\n", scan_code);
		}
		close(fd1);
	}
*/
