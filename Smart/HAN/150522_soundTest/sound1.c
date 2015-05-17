#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/soundcard.h>
#include <linux/soundcard.h>


char Buf[5000];
int count = 1024;

int main(void)
{
	int fd;
	int format;
	int stereo;
	int Rate;
	int status;
	

	if( (fd = open("/dev/dsp", O_RDWR, 0) ) == -1)
	{
		perror("OSS : error opening Device\n");
		return;
	}


	format = AFMT_S16_LE;
	if( ioctl(fd, SNDCTL_DSP_SETFMT, &format) == -1)
	{
		perror("SOUND_PCM_SETFMT");
		return -1;
	}

	stereo = 2;	//mono = 1, stereo = 2;
	if( ioctl(fd, SNDCTL_DSP_CHANNELS, &stereo) == -1)
	{
		perror("SOUND_PCM_CHANNELS");
		return -1;
	}

	Rate = 8000;
	if(ioctl(fd, SNDCTL_DSP_SPEED, &Rate) == -1)
	{
		perror("SOUND_PCM_SPEED");
		return -1;
	}
/*
	if(ioctl(fd, SNDCTL_DSP_SETDUPLEX, 0) == -1)
	{
		perror("SOUND_PCM_SETDUPLEX");
		return -1;
	}
*/
	//사운드 입력받기 위해서는 파일을 읽는 것과마 찬가지로 read()를 사용하낟.
	if((status = read(fd, Buf, count)) == -1)
	{
		perror("SOUND_READ_ERROR");
		return 0;
	}
	

	//사운드를 출력하기 위해서는 write()를 사용한다.
	/*
	if(status = write(fd, Buf, count)) == -1)
	{
		perror("SOUND_WRITE_ERROR");
		return FALSE;
	}
	*/

	printf("%s\n", Buf);

	return 0;
}
