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
		int deleted;
	} room;
   //static room* room_db;
   
   void *chat_func(void *db)
   {
		room* localR = (room*)db;
		localR->deleted = 0;
		//printf("x in thread is [%d] with port number [%s]\n",localR->x, localR->port_num);
		printf("created thread with port number [%s]\n",localR->port_num);
		int k = 0, m = 0, r = 0;
		int prev = 0;
		int members = 0;
		fd_set readfds;
		fd_set copyfds;
		char msg[256];
		int fsin_len2;
		struct sockaddr fsin2;
		fsin_len2 = sizeof(fsin2);
		struct timeval *timeout;
		printf("members: [%d]\n",localR->num_members);
	   for(;;)
	   {
		   members = localR->num_members;
		   if (members>0)
		   {
		   bzero(msg, strlen(msg));
		   FD_ZERO(&readfds);
		   //FD_ZERO(&copyfds);
		   if(members != prev)
		   {
			   
			   printf("someone trying to join, members = [%d]\n",members); //add to select monitor set
			   
			   fflush(stdout);
			   localR->slave_socket[members-1] = accept(localR->master_socket,(struct sockaddr*)&fsin2, &fsin_len2);
			   if (localR->slave_socket[members-1] < 0) printf("accept to chat fail\n");
			   else printf("join accept success\n");
				fflush(stdout);
			   //save to local select
			   //members++;
			   
		   }
		 
			   for(k=0;k<members;k++)
			   {
					FD_SET(localR->slave_socket[k], &readfds);
				   
			   }
		   prev = members;
		   if(localR->deleted == 1){
			   for (m=0;m<members;m++)
				{		
					fgets(msg, 256, "SERVER CLOSING DOWN");
					write(localR->slave_socket[m], msg, 256);		
				}
				sleep(2);
				for (m=0;m<members;m++)
				{		
					close(localR->slave_socket[m]);
					
				}
				close(localR->master_socket);
				bzero(localR->room_name, strlen(localR->room_name));
				pthread_exit(NULL);
			}
		   if(select(FD_SETSIZE, &readfds, NULL, NULL, NULL) == 0)//timeout?
			   printf("error with select\n");
		   else
		   {
			   for(k=0;k<members;k++)
			   {
					if (FD_ISSET(localR->slave_socket[k], &readfds))
					{
						bzero(msg, strlen(msg));
						r = read(localR->slave_socket[k], msg, 255);
						if (r > 0)
						{printf("client %d sent msg: %s in room %s\n", k, msg,localR->room_name);
						for (m=0;m<members;m++)
						{
							if (m!=k)
							{
								
								write(localR->slave_socket[m], msg, 256);
								
							}
								
						}
						}
						else if ( r == 0)
							printf("disconnected client\n");
						else
							printf("read error");
					}
				   
			   }
		   }
		   
	   }
	   }
   }

int main() {
  room room_db[MAX_ROOM];
  //room_db[0].x = 7;
  pthread_t chat_thread;
  //sem_t * sem;
  int pid;
  //sem = mmap(0, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  //sem_init(sem,1,1);
  
  char * service = "9645"; /* service name or port number */
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
	if (n > 0)
	printf("master message: [%s]\n",buffer);
	
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
				i=256;
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
				
				char* reply = "K";
				write(s_sock, reply, strlen(reply));
				
				if(pthread_create(&chat_thread, NULL, chat_func, &room_db[i]) != 0) {

					printf("Error creating thread\n");

				}
				i=256;
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
		
		for (i = 0; i < 256; ++i)
		{
			printf("comparing [%s] and [%s]\n",room_db[i].room_name,&buffer[5]);
			if(strcmp(room_db[i].room_name, (&buffer[5]))== 0){
			//if(strstr(buffer, room_db[i].room_name) != NULL && strcmp(room_db[i].room_name,"") != 0){
				printf("join Name exists\n");
				fflush(stdout);
				//sem_wait(sem);
				//printf("sem\n");
				fflush(stdout);
				name_exists= 1;
				//room_db[i].slave_socket[room_db[i].num_members] = accept(room_db[i].master_socket,(struct sockaddr*)&fsin, &fsin_len);
				char* portber = room_db[i].port_num;
				char strport[50] = "port ";
				strcat(strport, portber);
				printf("sending [%s] I IS %d\n",strport, i);
				write(s_sock, strport, strlen(strport));
				printf("members before inc [%d]\n",room_db[i].num_members);
				room_db[i].num_members = room_db[i].num_members + 1;
				printf("members after inc [%d]\n",room_db[i].num_members);
				fflush(stdout);
				i=256;
					//sem_post(sem);
					//n = read(room_db[i].slave_socket[room_db[i].num_members], buffer,255);
					//printf("message chat: %s\n",buffer);
					
			}
			else if(name_exists == 0 && strlen(room_db[i].room_name) == 0){
				//room not exists
				printf("join not exist\n");
				i=256;
			}
			else
				printf("join not exist? in i: [%d]\n", i);
		}
	}
	else if(buffer[0] == 'D' && buffer[1] == 'E' && buffer[2] == 'L' && 
	buffer[3] == 'E' && buffer[4] == 'T' && buffer[5] == 'E'){
		int name_exists = 0;
		int i;
		for(i = 0; i< 256; ++i){
			if(strcmp(room_db[i].room_name, (&buffer[7]))== 0){
				printf("Deleted Chatroom %s\n", room_db[i].room_name);
				fflush(stdout);
				name_exists = 1;
				room_db[i].deleted =1;
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
