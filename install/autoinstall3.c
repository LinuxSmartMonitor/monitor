#include <stdio.h>
#include <stdlib.h>

int main(void)
{
	//go to file path
	 
	//install 10-disp.conf file under xserver_conf folder to your X configure folder
	system("cd /home/pi/monitor/install/rpusbdisp_/drivers/linux-driver/ && sudo cp xserver_conf/10-disp.conf /usr/share/X11/xorg.conf.d/");

	//install uinput module - TEST IT
	system("cd /home/pi/monitor/install/rpusbdisp_/drivers/linux-driver/ && sudo modprobe uinput");

	//compile - fb0_final and kb_autostart and pi_main
	system("cd /home/pi/monitor/install && gcc -o fb0_final fb0_final.c");
	system("cd /home/pi/monitor/install && gcc -o kb_autostart kb_autostart.c");	
	system("cd /home/pi/monitor/rtl8192cu/WiFi_Direct_User_Interface && gcc -o pi_main ./pi_main.c ./pi_p2p_api.c ./pi_input.c ./pi_frame.c ./pi_onemore.c -lpthread -I/usr/include/");

	//directory move
	//copy autostart file
	system("cd /home/pi/monitor/install && sudo cp /home/pi/monitor/install/autostart /etc/xdg/lxsession/LXDE/");	

	//install ./sh execute	
	system("cd /home/pi/monitor/rtl8192cu/WiFi_Direct_User_Interface && sudo sh install.sh");

	//chmod 777 execution .exe file	
	system("cd /home/pi/monitor/rtl8192cu/WiFi_Direct_User_Interface && chmod 777 hostapd_cli");
	system("cd /home/pi/monitor/rtl8192cu/WiFi_Direct_User_Interface && chmod 777 hostapd");
	system("cd /home/pi/monitor/rtl8192cu/WiFi_Direct_User_Interface && chmod 777 wpa_supplicant");	
	system("cd /home/pi/monitor/rtl8192cu/WiFi_Direct_User_Interface && chmod 777 P2P_UI");
	system("cd /home/pi/monitor/rtl8192cu/WiFi_Direct_User_Interface && chmod 777 wpa_cli");
	system("cd /home/pi/monitor/rtl8192cu/WiFi_Direct_User_Interface && chmod 777 p2p_ui_test_2");

	system("sudo reboot");

	return 0;
}

