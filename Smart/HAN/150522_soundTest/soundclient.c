/* 
compilation step 

# gcc -o client client.c -lbluetooth -lpthread 

*/ 

#include <stdio.h> 
#include <stdlib.h> 
#include <signal.h> 
#include <pthread.h> 
#include <unistd.h> 
#include <sys/socket.h> 
#include <bluetooth/bluetooth.h> 
#include <bluetooth/rfcomm.h> 

#include <fcntl.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <linux/soundcard.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#define LENGTH 1		/* how many seconds of speech to store */
#define RATE 44100		/* the sampling rate */
#define SIZE 8				/* sample size: 8 or 16 bits */
#define CHANNELS 2		/* 1 = mono 2 = stereo */

/* this buffer holds the digitized audio */
char buf[LENGTH * RATE * SIZE * CHANNELS  /100];

int s , i; 
void ctrl_c_handler(int signal); 
void close_sockets(); 	/*socket close function*/
void *readMsg(); 			/*read function*/
void *sendMsg(); 			/*send function*/
FILE *fp;
FILE *ff;
int status1;
int fd;						/* sound device file descriptor */
int arg;					/* argument for ioctl calls */
int rate = RATE, format;

int main(int argc,char **argv){ 
fp = fopen("input.txt", "wb");  
//int status1;				/* return status of system calls */
 
int size;
  
  /* open sound device */
 fd = open ("/dev/dsp", O_RDWR);
 ff = fopen("pcm.wav","r");
	if(ff<=0)
	{
      perror ("open of pcm failed");
      exit (1);
    }



  if (fd < 0)
    {
      perror ("open of /dev/dsp failed");
      exit (1);
    }
     /* set sampling parameters */

				/* sample size */
  arg = SIZE;				
  status1 = ioctl (fd, SOUND_PCM_WRITE_BITS, &arg);
  if (status1 == -1)
    perror ("SOUND_PCM_WRITE_BITS ioctl failed");
  if (arg != SIZE)
    perror ("unable to set sample size");

				/* mono or stereo */
  arg = CHANNELS;		
  status1 = ioctl (fd, SOUND_PCM_WRITE_CHANNELS, &arg);
  if (status1 == -1)
    perror ("SOUND_PCM_WRITE_CHANNELS ioctl failed");
  if (arg != CHANNELS)
    perror ("unable to set number of channels");
				
				/* sampling rate */
  arg = RATE;			
  status1 = ioctl (fd, SOUND_PCM_WRITE_RATE, &arg);

    if (status1 == -1)
    perror ("SOUND_PCM_WRITE_WRITE ioctl failed");
             format = AFMT_S16_LE;
             if(ioctl(fd, SNDCTL_DSP_SETFMT, &format) == -1)
             {
                        perror("SOUND_PCM_SETFMT");
                        return -1;
             };
            if(ioctl(fd, SNDCTL_DSP_SPEED, &rate) == -1)
            {
                     	perror("SOUND_PCM_SPEED");
                     	return -1;
             };
       	     if(ioctl(fd, SNDCTL_DSP_SETDUPLEX, 0) == -1)        
             {
             		perror("SOUND_PCM_SETDUPLEX");
                        return -1;
             }



(void) signal(SIGINT,ctrl_c_handler); 

pthread_t readT, writeT; 
char *message1 = "Read thread\n"; 
char *message2 = "Write thread\n"; 
int iret1, iret2; 

struct sockaddr_rc addr= { 0 }; 

int status ; 
char dest[18] = "78:F7:BE:7B:74:30"; 
char msg[100]; 

//allocate a socket 
s = socket(AF_BLUETOOTH,SOCK_STREAM,BTPROTO_RFCOMM); 
addr.rc_family = AF_BLUETOOTH ; 
addr.rc_channel = 6; 
str2ba(dest,&addr.rc_bdaddr); 

//connect to server 
printf("going 2 connect\n"); 
status = connect(s,(struct sockaddr *)&addr,sizeof(addr)) ; 

/* loop until Control-C */
		//printf ("Say something:\n");
				
				/* record some sound */
      		//status1 = read (fd, buf, sizeof (buf));	
      		//if (status1 != sizeof (buf))
		//perror ("read wrong number of bytes");

		//printf ("You said:\n");

				/* play it back */
		//status1 = write (fd, buf, sizeof (buf));	
	      	//if (status1 != sizeof (buf))
		//perror ("wrote wrong number of bytes");
	      	
	/* wait for playback to complete before recording again */
	      //status1 = ioctl (fd, SOUND_PCM_SYNC, 0);
	      //if (status1 == -1)
		//	perror ("SOUND_PCM_SYNC ioctl failed");
 /* sound track */     


//send a message 
if(0 == status){ 
printf("connect success\n"); 

/* Create independent threads each of which will execute function */ 

//iret1 = pthread_create(&readT,NULL,readMsg,(void*) message1); 
iret2 = pthread_create(&writeT,NULL,sendMsg,(void*) message2); 

//pthread_join(readT,NULL); 
pthread_join(writeT,NULL); 

} 


close_sockets(); 
return 0; 
} 

			



void *sendMsg(){ 

char msg[13001] ; 
//int status ; 

do{
unsigned char new_arr[10]={0};
unsigned char new_buf[LENGTH * RATE * SIZE * CHANNELS  /100];
int size;
//memset(buf,0,sizeof(buf));
 
 		//printf ("Say something:\n");
      		//status1 = read (ff, buf, sizeof (buf));	/* record some sound */
      		//if (status1 != sizeof (buf))
		//	perror ("read wrong number of bytes");
printf("read Start\n");
while( (size =fread(buf,1, 1024, ff))>0 ){
	printf("sending size : %d\n", size);
status1 = send(s,buf,size,0); 
write (fd, buf, sizeof (buf));
}
printf("read Finish\n");

//sprintf(new_arr,"%d&_&%s",status1,buf);


//fscanf(fp, "%s", buf);

//status1 = send(s,buf,strlen(buf),0); 
//printf("len : %d\n", strlen(buf));

//printf("buf[0]=%d\n",buf[0]);

//printf("%s", buf);
//status = send(s,msg,strlen(msg),0); 

status1 = write (fd, buf, sizeof (buf));
int i;
fprintf(fp,"%s",buf);
//for(i=0; i<strlen(buf); i++)
//	fprintf(fp, "%d", buf[i]);
//for(int ipip=0; ipip<71000;  ipip++){

//	buf[ipip]=1;
//}
		 	

//fprintf(stdout,"Sound message = %s\n",new_arr); 
//fprintf(stdout,"Status1 = %d\n",status1); 
}while(status1 > 0); 
} 

//do{ 
//memset(msg,0,sizeof(msg)); 
//fgets(msg,24,stdin); 
//if(strncmp("EXIT",msg,4)==0 || strncmp("exit",msg,4)==0)break; 
//status = send(s,msg,strlen(msg),0); 
//fprintf(stdout,"Status = %d\n",status); 
//}while(status > 0); 
//} 


void *readMsg(){ 
int bytes_read; 
char buf[1024] = { 0 }; 
do{ 
//memset(buf,0,sizeof(buf)); 
//printf ("Say that:\n");
//status1 = read (fd, buf, sizeof (buf));	/* record some sound */
//    		if (status1 != sizeof (buf))
//			perror ("read wrong number of bytes");
//status1 = send(s,buf,strlen(buf),0); 
//fprintf(stdout,"Status1 = %d\n",status1); 
}while(1); 
} 

/*void *readMsg(){ 
int bytes_read
char buf[1024] = { 0 }; 
do{ 
memset(buf,0,sizeof(buf)); 
//read data from the client 
bytes_read = recv(s,buf,sizeof(buf),0) ; 
fprintf(stdout,"Bytes read = %d\n",bytes_read); 
if(bytes_read <= 0)break; 
fprintf(stdout,"<<>> %s",buf); 
}while(1); 
} */

void close_sockets(){ 
close(s); 
fprintf(stdout,"Close sockets\n"); 
} 

void ctrl_c_handler(int signal){ 
//fprintf(stdout,"Interrupt caught[NO: %d ]\n",signal); 
close_sockets(); 
exit(0); 
} 
