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
#include <sys/select.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/mman.h>

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

  typedef struct {
		char room_name[256];
		char port_num[20];
		int num_members;
		int slave_socket[MAX_MEMBER];
		fd_set * set;
		int master_socket;
		int process_id;
	} room;
   static room* room_db;

int main() {
  room_db = mmap(NULL, sizeof (*room_db)*MAX_ROOM, PROT_READ | PROT_WRITE, 
                    MAP_SHARED | MAP_ANONYMOUS, -1, 0);

  sem_t * sem;
  int pid;
  sem = mmap(0, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  sem_init(sem,1,1);
  
  char * service = "9145"; /* service name or port number */
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
		for (i = 0; i < 256; ++i){
			//check if room name exists
			if(strcmp(room_db[i].room_name, (&buffer[7]))== 0){
				printf("Name already exists\n");
				fflush(stdout);
				//set true
				name_exists= 1;
			}
			else if(name_exists == 0 && strlen(room_db[i].room_name) == 0){
				strncpy(room_db[i].room_name, &buffer[7], sizeof (room_db[i].room_name -1));
				printf("room created ");
				printf(room_db[i].room_name);
				fflush(stdout);
				
				//itoa(atoi(service)+i+1,room_db[i].port_num,10);
				sprintf(room_db[i].port_num, "%d", atoi(service)+i+1);
			printf("port num itoa: [%s] in i [%d]\n",room_db[i].port_num, i);
				fflush(stdout);
				room_db[i].master_socket = passiveTCPsock(room_db[i].port_num, 32);
				room_db[i].num_members = 0;
				i=256;
				char* reply = "K";
				write(s_sock, reply, strlen(reply));
			}
	}
	}
	else if(buffer[0] == 'J' && buffer[1] == 'O' && buffer[2] == 'I' && 
	buffer[3] == 'N'){
		printf(&buffer[5]);
			fflush(stdout);
		int i;
		//bool false
		int name_exists = 0;
		
		for (i = 0; i < 2; ++i)
		{
			printf("comparing [%s] and [%s]\n",room_db[i].room_name,&buffer[5]);
			if(strcmp(room_db[i].room_name, (&buffer[5]))== 0){
			//if(strstr(buffer, room_db[i].room_name) != NULL && strcmp(room_db[i].room_name,"") != 0){
				printf("join Name exists\n");
				fflush(stdout);
				sem_wait(sem);
				printf("sem\n");
				fflush(stdout);
				name_exists= 1;
				room_db[i].slave_socket[room_db[i].num_members] = accept(room_db[i].master_socket,(struct sockaddr*)&fsin, &fsin_len);
				char* portber = room_db[i].port_num;
				printf("sending [%s] I IS %d\n",portber, i);
				write(s_sock, portber, strlen(portber));
				room_db[i].num_members = room_db[i].num_members + 1;
				room_db[i].process_id = fork();
				if (room_db[i].process_id == 0) //child
				{
					if (room_db[i].num_members > 1)
					{
						printf("exiting child\n");
						sem_post(sem);
						_exit(0);
						printf("after exit?\n");
					}
					printf("in child\n");
					fflush(stdout);
					sem_post(sem);
					n = read(room_db[i].slave_socket[room_db[i].num_members], buffer,255);
					printf("message chat: %s\n",buffer);
					
					
					_exit(0);
				}
			}
			else if(name_exists == 0 && strlen(room_db[i].room_name) == 0){
				//room not exists
				printf("join not exist\n");
			}
			else
				printf("join not exist in i: [%d]\n", i);
		}
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
