#include <stdio.h>
#include <stdlib.h>


int main(void)
{
	//go to file path
	//system("cd /home/pi/monitor/install/rpusbdisp_/drivers/linux-driver/ && make");

	//install the kernel module to module folder and update the module dependence relationship
	system("cd /home/pi/monitor/install/rpusbdisp_/drivers/linux-driver/ && install rp_usbdisplay.ko /lib/modules/`uname -r`/kernel/rp_usbdisplay.ko");

	system("cd /home/pi/monitor/install/rpusbdisp_/drivers/linux-driver/ && depmod -a");
	
	//modprobe command to load rp_usbdisplay kernel module
	system("cd /home/pi/monitor/install/rpusbdisp_/drivers/linux-driver/ && modprobe rp_usbdisplay");

	//Configure the kernel module to be loaded automatically and reboot
	system("cd /home/pi/monitor/install/rpusbdisp_/drivers/linux-driver/ && echo rp_usbdisplay>>/etc/modules");
	
	printf("hi!!!!!");

	//Reboot it
	system("sudo reboot");

	return 0;
}

