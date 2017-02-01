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
#include <pthread.h>

#define MAX_ROOM 256
#define MAX_MEMBER 256

//from slides, does socket, bind, listen
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

//structure for room to be passed into chat threads
typedef struct {
	char room_name[256];
	char port_num[20];
	int num_members;
	int slave_socket[MAX_MEMBER];
	int master_socket;
	int deleted;
} room;

//chatroom thread function
void *chat_func(void *db)
{
	room* localR = (room*)db;
	localR->deleted = 0;
	printf("created thread with port number [%s]\n",localR->port_num);
	int k = 0, m = 0, n = 0, r = 0;
	int prev = 0;
	int members = 0;
	fd_set readfds;
	struct timeval time;
	char msg[256];
	int fsin_len2;
	struct sockaddr fsin2;
	fsin_len2 = sizeof(fsin2);
	printf("members: [%d]\n",localR->num_members);
	for(;;)
	{
	   time.tv_sec = 2;
	   time.tv_usec = 0;
	   members = localR->num_members;
	    if(localR->deleted == 1){			//check if deleting room
			   for (m=0;m<members;m++)
				{		
					sprintf(msg, "Chatroom shutting down", 256);
					write(localR->slave_socket[m], msg, 256);	//tell clients shutting down	
				}
				sleep(2);
				for (m=0;m<members;m++)
				{		
					close(localR->slave_socket[m]);
					
				}
				close(localR->master_socket);
				bzero(localR->room_name, strlen(localR->room_name));
				printf("deleted chat room\n");
				fflush(stdout);
				pthread_exit(NULL);
			}
	   if (members>0)						//check if anyone in room
	   {	
		   bzero(msg, strlen(msg));
		   FD_ZERO(&readfds);
		   if(members != prev)				//check if a new member joined
		   {
			   printf("someone trying to join, members = [%d]\n",members); 
			   fflush(stdout);				//add new connection
			   localR->slave_socket[members-1] = accept(localR->master_socket,(struct sockaddr*)&fsin2, &fsin_len2);
			   if (localR->slave_socket[members-1] < 0) printf("accept to chat fail\n");
			   else printf("join accept success\n");
			   fflush(stdout);
		   }	 
		   for(k=0;k<members;k++)			//add all connections to file descriptor set for monitoring
				FD_SET(localR->slave_socket[k], &readfds);
		   prev = members;
		   if(select(FD_SETSIZE, &readfds, NULL, NULL, &time) == -1)
			   printf("error with select\n");
		   else
		   {
			   for(k=0;k<members;k++)
			   {							//loop thru all connections to check which client sent message
					if (FD_ISSET(localR->slave_socket[k], &readfds))
					{
						bzero(msg, strlen(msg));
						r = read(localR->slave_socket[k], msg, 255);
						if (r > 0)
						{
							printf("client %d sent msg: %s in room %s\n", k, msg,localR->room_name);
							for (m=0;m<members;m++)
								if (m!=k)	//send client's message to all other members
									write(localR->slave_socket[m], msg, 256);
						}
						else if ( r == 0)	//no bytes read, close the client
						{
							printf("disconnected client\n");//use FD_CLEAR?
							close(localR->slave_socket[k]);
							for (n=k;n<members-1;n++) //shift array to left
								localR->slave_socket[n] = localR->slave_socket[n+1];
							--(localR->num_members);
							members--;
							prev = members;
						}
						else
							printf("read error");
					}  
				}
			}
		}
	}
}
// ./a.out port
int main(int argc, char *argv[]) {
  room room_db[MAX_ROOM];
  pthread_t chat_thread;
  char * service = argv[1]; /* service name or port number */
  int    m_sock, s_sock;   /* master and slave socket     */
  char buffer[256];
  int n;
  m_sock = passiveTCPsock(service, 32);
  int fsin_len;
  struct sockaddr fsin;
  fsin_len = sizeof(fsin);
  
  for (;;) {		//infinite loop to accept multiple client commands
    s_sock = accept(m_sock,(struct sockaddr*)&fsin, &fsin_len);
	if (s_sock < 0) printf("accept failed");
	n = read(s_sock, buffer,255);
	if (n > 0)
		printf("master message: [%s]\n",buffer);	//check for CREATE command
	if(buffer[0] == 'C' && buffer[1] == 'R' && buffer[2] == 'E' && 
	   buffer[3] == 'A' && buffer[4] == 'T' && buffer[5] == 'E'){
		printf(&buffer[7]);
		fflush(stdout);
		int i;
		int name_exists = 0;
		for (i = 0; i < 256; ++i){
			//check if room name exists
			if(strcmp(room_db[i].room_name, (&buffer[7]))== 0){
				printf("Name already exists\n");
				fflush(stdout);
				//set true
				name_exists= 1;
				i=256;
			}		//create the room
			else if(name_exists == 0 && strlen(room_db[i].room_name) == 0){
				strncpy(room_db[i].room_name, &buffer[7], sizeof (room_db[i].room_name -1));
				printf("room created [%s]",room_db[i].room_name);
				fflush(stdout);
				sprintf(room_db[i].port_num, "%d", atoi(service)+i+1);
				printf("port number is: [%s]\n",room_db[i].port_num);
				fflush(stdout);
				room_db[i].master_socket = passiveTCPsock(room_db[i].port_num, 32); //make a master socket for the room with a new port
				room_db[i].num_members = 0;
				char* reply = "server created chatroom";
				write(s_sock, reply, strlen(reply));
				if(pthread_create(&chat_thread, NULL, chat_func, &room_db[i]) != 0) //create the room thread
					printf("Error creating thread\n");
				i=256;
			}
		}
	}				//check for command JOIN
	else if(buffer[0] == 'J' && buffer[1] == 'O' && buffer[2] == 'I' && 
	        buffer[3] == 'N'){
		printf(&buffer[5]);
		fflush(stdout);
		int i;
		//bool false
		int name_exists = 0;
		for (i = 0; i < 256; ++i)
		{
			//printf("comparing [%s] and [%s]\n",room_db[i].room_name,&buffer[5]);
			if(strcmp(room_db[i].room_name, (&buffer[5]))== 0){ //joing room
				printf("joining...\n");
				fflush(stdout);
				name_exists= 1;
				char* portber = room_db[i].port_num;
				char strport[50] = "port ";
				strcat(strport, portber);
				printf("sending [%s]\n",strport);
				write(s_sock, strport, strlen(strport)); //send the room's port number
				//printf("members before inc [%d]\n",room_db[i].num_members);
				room_db[i].num_members = room_db[i].num_members + 1; //signals new client incoming
				//printf("members after inc [%d]\n",room_db[i].num_members);
				fflush(stdout);
				i=256;
					
			}
			else if(name_exists == 0 && strlen(room_db[i].room_name) == 0){
				//room not exists
				printf("join room not exist\n");
				i=256;
			}
			else
				printf("join not exist? in i: [%d]\n", i);
		}
	}					//check for command DELETE
	else if(buffer[0] == 'D' && buffer[1] == 'E' && buffer[2] == 'L' && 
	        buffer[3] == 'E' && buffer[4] == 'T' && buffer[5] == 'E'){
		int name_exists = 0;
		int i;
		for(i = 0; i< 256; ++i){//find room 
			if(strcmp(room_db[i].room_name, (&buffer[7]))== 0){
				printf("Deleting Chatroom %s\n", room_db[i].room_name);
				fflush(stdout);
				name_exists = 1;
				room_db[i].deleted =1; //signal thread for deletion
				i = 256;
			}
		}
		if(name_exists== 0){
			printf("Chatroom does not exist\n");
			fflush(stdout);
		}	
	}
	memset(buffer, '\0', 256);
    close(s_sock);
  }
}
