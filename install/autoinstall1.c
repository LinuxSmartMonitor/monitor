#include <stdio.h>
#include <stdlib.h>


int main(void)
{
	//root password configure 
	system("sudo passwd root");

	//rc.local ->raspberry auto booting copy it
	system("sudo cp /home/pi/monitor/install/rc.local /etc/");

	//kernel update it
	system("sudo apt-get update");

	//install new kernel version
	system("sudo apt-get install linux-image-rpi-rpfv linux-headers-rpi-rpfv");

	//copy the config file
	system("sudo cp /home/pi/monitor/install/config.txt /boot/");

	system("sudo reboot");

	return 0;
}

