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
#define MAX_ROOM 256
#define MAX_MEMBER 256

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

  typedef struct {
		char room_name[256];
		int port_num;
		int num_members;
		int slave_socket[MAX_MEMBER];
		int process_id;
	} room;
   room room_db[MAX_ROOM];

  char * service = "9114"; /* service name or port number */
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
	//if (fork() == 0) {    child 
     // close(m_sock);
      //handle request here . . .
    //}
    //close(s_sock);

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
	*/
	if(buffer[0] == 'C' && buffer[1] == 'R' && buffer[2] == 'E' && 
	buffer[3] == 'A' && buffer[4] == 'T' && buffer[5] == 'E'){
			printf(&buffer[7]);
			fflush(stdout);
		int i;
		//bool false
		int name_exists = 0;
		for (i = 0; i < 256; ++i)
			if(strcmp(room_db[i].room_name, (&buffer[7]))== 0){
				printf("Name already exists\n");
				fflush(stdout);
				name_exists= 1;
			}
			else if(name_exists == 0 && strlen(room_db[i].room_name) == 0){
				strncpy(room_db[i].room_name, &buffer[7], sizeof (room_db[i].room_name -1));
				printf("room created ");
				printf(room_db[i].room_name);
				fflush(stdout);
				i=256;
			}
	}
	else if(buffer[0] == 'J' && buffer[1] == 'O' && buffer[2] == 'I' && 
	buffer[3] == 'N'){
		printf("join\n");
		fflush(stdout);
	}
	else if(buffer[0] == 'D' && buffer[1] == 'E' && buffer[2] == 'L' && 
	buffer[3] == 'E' && buffer[4] == 'T' && buffer[5] == 'E'){
		printf("delete\n");
		fflush(stdout);
	}
	
	memset(buffer, '\0', 256);
    close(s_sock);
  }
}