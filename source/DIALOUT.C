#include <types.h>
#include <netinet\in.h>
#include <sys\socket.h>
#include <sys\select.h>
#include <netdb.h>
#include <stdio.h>
#include <nerrno.h>
#include <ctype.h>
#include <os2def.h>

/*#include <signal.h> */
/*#include <string.h> */
/*#include <io.h> */

int s;                      /* The current socket # */

struct sockaddr_in saddr;

int wport = 7551;           /* dialin PORT num */

unsigned char *buffer,*destsite,*temp,*dumpin;

extern unsigned long int codec(unsigned long int howmany);

int getconn(char *host, int port);
int connectit();
int send_input(unsigned char *buf, unsigned long length);


unsigned long int y,cat,end;


FILE *outwav,*out1,*out2;

void noroute()
{
      printf("\a  \a  \a  \a  \a");
}
void serverstop()
{
    printf("\a  \a  \a  \a");
}

int main( int argc, char **argv)
{
  fd_set readfds, exceptfds;
  long int x,z;
  int inbyte,strip;
  unsigned long start,cur,sending,blocks;
  unsigned char buff[3000];
  char flag = 0;

  outwav = fopen("out.wav","rb");
  rewind(outwav);
  fseek(outwav,0,SEEK_END);
  end = ftell(outwav);
  rewind(outwav);
  fclose(outwav);
  if ( end > 300000)
    {
     printf("DialOut: Input file to large\a\a\r\n");remove("out.wav");
     return(1);
    }
  if ( ( buffer = (unsigned char *) malloc(end+1)) == NULL )
    {
     printf("DialOut: No memory for buffer\a\a\r\n");remove("out.wav");
     return(1);
    }
  if ( ( temp = (unsigned char *) malloc((end/2)+1024)) == NULL )
    {
     printf("DialOut: No memory for buffer\a\a\r\n");remove("out.wav");
     return(1);
    }
  if ( ( dumpin = (unsigned char *) malloc((end/2)+1)) == NULL )
    {
     printf("DialOut: No memory for buffer\a\a\r\n");remove("out.wav");
     return(1);
    }

  s = 128;    /* dummy value so we don't try to shutdown stdin */

  sprintf(buff,"%s",argv[1]);
  destsite = buff;

  s = getconn(destsite, wport);

  if ( s == -1 )
    {
     noroute();
     remove("out.wav");
     return ( -2 );
    }

  FD_ZERO(&readfds);
  FD_ZERO(&exceptfds);
  FD_SET(0, &readfds);
  FD_SET(s, &readfds);
  FD_SET(s, &exceptfds);

  send_input("cHaT",5);

  while ( flag == 0 )
     {
      inbyte = recv(s, buff, 2, MSG_PEEK);
      if ( inbyte == 2 ) flag = 1;
      if ( inbyte == -1 ) { serverstop(); remove("out.wav"); return(-1); }
     }
  flag = 0;
  recv(s, buff, 2, 0x0);

  outwav = fopen("out.wav","rb");
  for (x=0;x<44;x++) buff[x] = fgetc(outwav);
  send_input(buff,44);

  while ( flag == 0 )
     {
      inbyte = recv(s, buff, 2, MSG_PEEK);
      if ( inbyte == 2 ) flag = 1;
      if ( inbyte == -1 ) { serverstop(); remove("out.wav"); return(-1); }
     }
  flag = 0;
  recv(s, buff, 2, 0x0);

  sprintf(buff,"%-s",argv[2]);
  send_input(buff,4);

  strip = atoi(buff);

  while ( flag == 0 )
     {
      inbyte = recv(s, buff, 2, MSG_PEEK);
      if ( inbyte == 2 ) flag = 1;
      if ( inbyte == -1 ) { serverstop(); remove("out.wav"); return(-1); }
     }
  flag = 0;
  recv(s, buff, 2, 0x0);

  sprintf(buff,"%s",argv[3]);
  send_input(buff,20);

  while ( flag == 0 )
     {
      inbyte = recv(s, buff, 2, MSG_PEEK);
      if ( inbyte == 2 ) flag = 1;
      if ( inbyte == -1 ) { serverstop(); remove("out.wav"); return(-1); }
     }
  flag = 0;
  recv(s, buff, 2, 0x0);

  fseek(outwav,44,SEEK_SET);
  cur = 44;
  fseek(outwav,0,SEEK_END);
  end = ftell(outwav);
  start = end - cur;
  fseek(outwav,cur,SEEK_SET);

  fread(buffer,start,1,outwav);
  fclose(outwav);

  y=0;
  for (x=0;x<start;x+=2)
    {
     buffer[x] &= 0xFE;
     dumpin[y++]=z;
    }

  cat = codec(y);

  sprintf(buff,"%6ld",cat);
  send_input(buff,7);

  while ( flag == 0 )
     {
      inbyte = recv(s, buff, 2, MSG_PEEK);
      if ( inbyte == 2 ) flag = 1;
      if ( inbyte == -1 ) { serverstop(); remove("out.wav"); return(-1); }
     }
  flag = 0;
  recv(s, buff, 2, 0x0);

  y=0;
  for(x=0;x<(cat/2048);x++)
     {
      for(z=0;z<2048;z++) buff[z] = temp[z+y];

      send_input(buff,2048);

      y+=2048;

      while ( flag == 0 )
         {
          inbyte = recv(s, buff, 2, MSG_PEEK);
          if ( inbyte == 2 ) flag = 1;
          if ( inbyte == -1 ) { serverstop(); remove("out.wav"); return(-1); }
         }
      flag = 0;
      recv(s, buff, 2, 0x0);
     }

  z = cat/2048;
  sending = z*2048;
  blocks = cat - sending;
  sprintf(buff,"%6ld",blocks);
  send_input(buff,7);

  while ( flag == 0 )
     {
      inbyte = recv(s, buff, 2, MSG_PEEK);
      if ( inbyte == 2 ) flag = 1;
      if ( inbyte == -1 ) { serverstop(); remove("out.wav"); return(-1); }
     }
  flag = 0;

  recv(s, buff, 2, 0x0);


  if ( blocks != 0 )
    {
      for(z=0;z<blocks;z++) buff[z] = temp[z+y];

      send_input(buff,blocks);

      y+=blocks;


      while ( flag == 0 )
         {
          inbyte = recv(s, buff, 2, MSG_PEEK);
          if ( inbyte == 2 ) flag = 1;
          if ( inbyte == -1 ) { serverstop(); remove("out.wav"); return(-1); }
         }
      flag = 0;

      recv(s, buff, 2, 0x0);

    }


  remove("out.wav");


  shutdown(s, 2);   /* who cares- were outta here. */
  soclose(s);         /* ditto. */

 return(0);
}

int getconn(char *host, int port)
{
    struct hostent *temp;

    if(isdigit(*host))
    {
        if((saddr.sin_addr.s_addr = inet_addr(host)) == (u_short)INADDR_NONE)
        {
            return -1;
        }
    }
    else
    {
    if((temp = gethostbyname(host)) == (struct hostent *)0)
        {
	    return -1;
        }
    memcpy((char *)&(saddr.sin_addr),(char *)temp->h_addr,sizeof(struct in_addr));
    }
    saddr.sin_family=(short)AF_INET;
    saddr.sin_port = htons((u_short)port);
    return connectit();
}

int connectit()
{
    int s;

    if((s = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        return -1;
    }
    if(connect(s,(struct sockaddr *)&saddr,sizeof(struct sockaddr_in)) == -1)
    {
        return -1;
    }
    else
    {
        return s;
    }
} 


int send_input(unsigned char *buf, unsigned long length)
{
    send(s , buf, length,0x0);
    return(0);
}
