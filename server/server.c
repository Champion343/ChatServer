#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>

int passiveTCPsock(const char * service, int backlog) {
  
  struct sockaddr_in sin;          /* Internet endpoint address */
  memset(&sin, 0, sizeof(sin));    /* Zero out address */
  sin.sin_family      = AF_INET;
  sin.sin_addr.s_addr = INADDR_ANY;
  char buffer[256];
  
  /* Map service name to port number */
  struct servent * pse;
  if (pse = getservbyname(service, "tcp") )
      sin.sin_port = pse->s_port;
  else if ((sin.sin_port = htons((unsigned short)atoi(service))) == 0)
      printf("can’t get <%s> service entry\n");

  /* Allocate socket */
  int s = socket(AF_INET, SOCK_STREAM, 0);
  if (s < 0) printf("can’t create socket: %s\n");

  /* Bind the socket */
  int bindnum;
  if ((bindnum = bind(s, (struct sockaddr *)&sin, sizeof(sin))) < 0)
       printf("%d\n", bindnum);
  
  /* Listen on socket */
  if (listen(s, backlog) < 0)
       printf("can’t listen on …\n");
  
  return s;
}

int main() {

  /*typedef struct {
		char room_name[256];
		int port_num;
		int num_members;
		int slave_socket[MAX_MEMBER];
	} room;
   room room_db[MAX_ROOM];*/

  char * service = "9115"; /* service name or port number */
  int    m_sock, s_sock;   /* master and slave socket     */
  char buffer[256];
  int n;
  //service = argv[1];
  m_sock = passiveTCPsock(service, 32);
  int fsin_len;
  struct sockaddr fsin;
  fsin_len = sizeof(fsin);
  for (;;) {
    s_sock = accept(m_sock,(struct sockaddr*)&fsin, &fsin_len);
    if (s_sock < 0) printf("accept failed");
/*	if (fork() == 0) {    child 
      close(m_sock);
      /* handle request here . . . 
    }
    close(s_sock);
*/
    /*time_t now;
    time(&now);
    char * pts = ctime(&now);
    write(s_sock, pts, strlen(pts));*/
    n = read(s_sock, buffer,255);
	printf("message: %s\n",buffer);
	
	/*buffer[strlen(buffer) - 2] = '\0';
	int cmp = strcmp(buffer, "CREATE\n");
	printf("%d\n", cmp);
	printf(buffer[0]);
	if(&buffer[0] == "C" && &buffer[1] == "R" && &buffer[2] == "E" && 
	&buffer[3] == "A" && &buffer[4] == "T" && &buffer[5] == "E"){
		printf("created");
		fflush(stdout);
	}*/
	
	memset(buffer, '\0', 256);
    close(s_sock);
  }
}