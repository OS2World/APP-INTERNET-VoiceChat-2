#include <types.h>
#include <netinet\in.h>
#include <sys\socket.h>
#include <sys\select.h>
#include <netdb.h>
#include <stdio.h>
#include <nerrno.h>
#include <ctype.h>
#include <os2def.h>

#ifndef errno
extern int errno;
#endif


extern unsigned long int dedec(unsigned long int length);


int conn_socket;           /* also the line number */
short conn_port;           /* port number on remote host */
char conn_onfrom[50];      /* where they are on from */


int sock;       /* our initial socket */
int conn;       /* connection */


unsigned char recvbuf[3000],sendbuf[3000],mode,*buffer,*temp,*dumpin,header[45],nick[50],drop[100],IP_[20],*working;

unsigned long y=0,cat,dog,total;

int doit( int port);
int newconn(int s);
int getsock(char *address, int port);
int infromsock(int s, unsigned char *buffer, int size);
int outtosock(int s, char *buffer);
int acceptcon(int s, char *from,int *port);


FILE *inwav,*hostfile,*in1,*in2;


int main(void)
{

   if ( ( buffer = (unsigned char *) malloc(600000)) == NULL )
         {
          printf("DialOut: No memory for buffer\r\n");
          return(1);
         }
   if ( ( temp = (unsigned char *) malloc(307000)) == NULL )
      {
       printf("DialOut: No memory for buffer\r\n");
       return(1);
      }
   if ( ( dumpin = (unsigned char *) malloc(307000)) == NULL )
      {
       printf("DialOut: No memory for buffer\r\n");
       return(1);
      }


   doit(7551); /* Start the daemon with port 113 ( the IDENTD socket ) */
 return(-1);
}
	



int doit( int port)
{
  fd_set needread;        /* for seeing which fds we need to read from */

    sock = getsock(sendbuf, port);

    printf("ษอออออออออออออออออออออออออออป\r\n");
    printf("บ    (dialin.exe)  v0.08a   บ\r\n");
    printf("บ  compiled with Cset v2.1  บ\r\n");
    printf("บ  (c) 1995 Stephen Loomis  บ\r\n");
    printf("ศอออออออออออออออออออออออออออผ\r\n");

    while (1 == 1)
        {
         FD_ZERO(&needread);
         FD_SET(sock, &needread);

         if (select(sizeof(fd_set), &needread, (fd_set *)0,
              (fd_set *)0, (struct timeval *)0) == -1)
           {
            if (errno != EINTR)
                {
                 continue;
                }
            else
             continue; /* do it again and get it right */
           }
        if (FD_ISSET(sock, &needread))
        {
         y=0;cat=0;dog=0;total=0;
         conn = newconn(sock);
        }
 
    }
}


int newconn(int s)
{
    int port, i,count;
    unsigned long expect,x;
    unsigned char errflag = 0;


    conn_socket = acceptcon(s, conn_onfrom, &port);
    conn_port = port;


    if ( infromsock(conn_socket, recvbuf, 5) == -1)
                 {
                  (void) soclose(conn_socket);
                  return(0);
                 }

    if ( recvbuf[0] != 'c' )
                {
                  (void) soclose(conn_socket);
                  return(0);
                }

    send(conn_socket,"A", 2, 0x0);

    if ( infromsock(conn_socket, recvbuf, 44) == -1)
                 {
                  shutdown(conn_socket, 2);
                  (void) soclose(conn_socket);
                  return(0);
                 }

   memcpy(header,recvbuf,44);

   send(conn_socket,"A", 2, 0x0);

   if ( infromsock(conn_socket, recvbuf, 4) == -1)
                 {
                  shutdown(conn_socket, 2);
                  (void) soclose(conn_socket);
                  return(0);
                 }

   mode = atoi(recvbuf);

   send(conn_socket,"A", 2, 0x0);

   if ( infromsock(conn_socket, recvbuf, 20) == -1)
                 {
                  shutdown(conn_socket, 2);
                  (void) soclose(conn_socket);
                  return(0);
                 }

   sprintf(nick,"%s",recvbuf);

   send(conn_socket,"A", 2, 0x0);

   if ( infromsock(conn_socket, recvbuf, 7) == -1)
                 {
                  shutdown(conn_socket, 2);
                  (void) soclose(conn_socket);
                  return(0);
                 }

   total = expect = atoi(recvbuf);

   send(conn_socket,"A", 2, 0x0);
   y=0;

   for (i=0;i<expect/2048;i++)
   {
    errflag = 1;
    while ( errflag == 1 )
       {
        count = recv(conn_socket, temp, 2048, MSG_PEEK);
        if ( count == -1 )
            {
             shutdown(conn_socket, 2);
             (void) soclose(conn_socket);
             return(0);
            }
        if ( count == 2048 ) errflag = 0;
        }
    if ( infromsock(conn_socket, temp, 2048) == -1)
                 {
                  shutdown(conn_socket, 2);
                  (void) soclose(conn_socket);
                  return(0);
                 }
    for(x=0;x<2048;x++)
            {
             dumpin[y] =  temp[x];
             y++;
            }
    send(conn_socket,"A", 2, 0x0);
    }


   if ( infromsock(conn_socket, recvbuf, 7) == -1)
                 {
                  shutdown(conn_socket, 2);
                  (void) soclose(conn_socket);
                  return(0);
                 }

   expect = atoi(recvbuf);

   send(conn_socket,"A", 2, 0x0);

   if ( expect != 0 )
        {
        errflag = 1;
        while ( errflag == 1 )
           {
            count = recv(conn_socket, temp, expect, MSG_PEEK);
            if ( count == -1 )
                {
                 shutdown(conn_socket, 2);
                 (void) soclose(conn_socket);
                 return(0);
                }
            if ( count == expect ) errflag = 0;
            }
        if ( infromsock(conn_socket, temp, expect) == -1)
                     {
                      shutdown(conn_socket, 2);
                      (void) soclose(conn_socket);
                      return(0);
                     }
        for(x=0;x<expect;x++)
                {
                 dumpin[y] = temp[x];
                 y++;
                }
        send(conn_socket,"A", 2, 0x0);
       }

   expect = dedec(y);

   y=0;
   for (x=0;x<(expect*2);x+=2)
    {
     buffer[x] = buffer[x+1]  = temp[y++];
    }

   remove("in.wav");

   sprintf(drop,"%s %s %s\r\n",nick,conn_onfrom,IP_);

   hostfile = fopen("host.txt","wb");
   fwrite(drop,80,1,hostfile);
   fclose(hostfile);

   inwav = fopen("in.wav","wb");
   fwrite(header,44,1,inwav);
   fwrite(buffer,(expect*2),1,inwav);
   fclose(inwav);

   shutdown(conn_socket, 2);
   (void) soclose(conn_socket);

   return 0;
}


int getsock(char *address, int port)
{
    int s,one = 1;
    struct in_addr tmpaddr;
    struct sockaddr_in saddr;
    struct hostent *tmphost;
    
    /* get a fresh socket */
    if ((s = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        {
         perror("\a");
         exit(errno);
        }
    
    saddr.sin_addr.s_addr = INADDR_ANY;
    saddr.sin_family = AF_INET;
    saddr.sin_port = (unsigned short) htons((unsigned short) port);
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *)&one, sizeof(one));
    
    /* attempt to bind the socket to our address */
    if (bind(s, (struct sockaddr *) &saddr, sizeof(struct sockaddr_in)) == -1)
	{
        perror("\a");
        exit(errno);
	}
    
    /* set our backlog of waiting connections to 5(max the system allows) */
    if (listen(s, 5) == -1)
        {
         perror("\a");
         exit(errno);
        }
    return s;
}

int acceptcon(int s, char *from,int *port)
{
    int ns;
    int length = sizeof(struct sockaddr_in);
    unsigned char *buf;
    struct hostent *tmphost;
    struct sockaddr_in saddr;
    
    if ((ns = accept(s, (struct sockaddr *) &saddr, &length)) == -1)
        {
         return -1;
        }
    
    tmphost = gethostbyaddr((char *) &(saddr.sin_addr),
			    sizeof(struct in_addr), AF_INET);
    if (tmphost == (struct hostent *)0)
        {
        (void) strcpy(from, "UNKNOWN");
        (void) strcpy(from, inet_ntoa(saddr.sin_addr));
        }
    else
        {
        buf = tmphost->h_name;
        if (buf != (unsigned char *)0)
            {
            (void) strcpy(from, buf);
            }
        else
            (void) strcpy(from, inet_ntoa(saddr.sin_addr));
        }
    (void) strcpy(IP_, inet_ntoa(saddr.sin_addr));

    *port = ntohs(saddr.sin_port);
    
    return ns;
}

int infromsock(int s, unsigned char *buffer, int size)
{
  unsigned int numchars;
  
  if ((numchars = recv(s, buffer, size, 0x0)) <= 0 )
        { /* panic for now */
        if( errno == 12 )
            return 0;
        else
            return -1;
        }
  
  if (numchars == 0)
        { /* foreign end closed the connection be graceful */
         return -1; /* tell the calling proc to deal with it. */
        }
  buffer[numchars] = '\000';
  return numchars;
}

int outtosock(int s, char *buffer)
{
  int length, numsent;
  
  length = strlen(buffer);
  if ((numsent = send(s, buffer, length, 0x0)) == -1)
    {
     return -1;
    }
  else if (numsent < length) {}
  
  return 0;
}
