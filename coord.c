#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
#include <linux/input.h>
#include <errno.h>

int main (int argc, char *argv[]){

	struct input_event event,event2,event3, event_end;
	int sim_fd;
	int x,y;
	char buffer[10];
	int cnt = 1;
	int fd,fd2;
	int nBytes;
	// sysfs 
	sim_fd = open("/sys/devices/platform/vms/coordinates", O_RDWR);
	//sim_fd = open("/sys/devices/platform/virmouse/vmevent", O_RDWR);

/*
	fd = open("/dev/input/event3", O_RDWR);
	fd2 = open("/dev/input/event2", O_RDWR);
	
	if(fd < 0) {
		printf("Error open mouse:%s", strerror(errno));
		return -1;
	}
*/
	if(sim_fd < 0) {
			perror("Couldn't open vms coordinate file");
	}

	memset(&event, 0, sizeof(event));
	memset(&event2, 0, sizeof(event2));
	//memset(&event3, 0, sizeof(event3));
	memset(&event_end, 0, sizeof(event_end));
	gettimeofday(&event.time, NULL);
	gettimeofday(&event_end.time, NULL);
	while(1) {
		
			//  generating random coordinate
			x = random()%20;
			y = random()%20;
			if (x%2) x = -x;
			if (y%2) y = -y;
			sprintf (buffer, "%d %d %d",x,y, 1);
			write(sim_fd, buffer, strlen(buffer));
			//nBytes = read(fd, &event, sizeof(event));
			//printf("%d\n", nBytes);
			//write(fd2, &nBytes, sizeof(event));
			fsync(sim_fd);
/*
			//move coordinate
			event.type = EV_REL;
			event.code = REL_X;
			event.value = x;
			event2.type = EV_REL;
			event2.code = REL_Y;
			event2.value = y;
			//event3.type = EV_KEY;
			//event3.code = BTN_0;
			event_end.type = EV_SYN;
			event_end.code = SYN_REPORT;
			event_end.value = 100;
	
			
			write(fd, &event, sizeof(event)); 
			write(fd, &event2, sizeof(event2));
			//write(fd, &event3, sizeof(event3));
			write(fd, &event_end, sizeof(event_end));
*/
			sleep(1);

			printf("BJ count : %d, x = %d, y = %d\n",cnt,x,y);
			cnt++;
			
	}
	//close(fd);
	close(sim_fd);
}
