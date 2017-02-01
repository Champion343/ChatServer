#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <sys/time.h>
#define bufmax 100

// ./a.out host_address port
int main(int argc, char *argv[]) {

struct sockaddr_in d, d1;
int sock, room_sock;
int join_port, exit;
char buffer[256],  buf[256];
int port = atoi(argv[2]);
char *serv_addr = argv[1];
int m, n, sel, max_fd;
fd_set read_fds;

//Client loop
for(;;){
	exit = 0;
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock < 0)
		printf("unable to open socket");
	
    bzero(&d, sizeof(d));
    d.sin_family = AF_INET;
    d.sin_port = htons(port);
	
    if ( inet_aton(serv_addr, &d.sin_addr.s_addr) == 0 )
        printf("error\n");
	
	printf("CREATE, JOIN or DELETE a room\n");
	fflush(stdout);
	bzero(buffer, 256);
	fgets(buffer,255,stdin);
	//connect to server
    if ( connect(sock, (struct sockaddr*)&d, sizeof(d)) != 0 )
        printf("failed to connect to server\n");
	//write to server
    n = write(sock, buffer, strlen(buffer));
	
	bzero(buffer, 256);
	n = read(sock, buffer, 255);
	printf("%s\n", buffer);
    close(sock);
	
	//Join room
	if(buffer[0] == 'p'){
		char *temp_str;
		temp_str = &buffer[5];
		//convert port	
		join_port = atoi(temp_str);
		room_sock = socket(AF_INET, SOCK_STREAM, 0);
		if(room_sock < 0)
			printf("unable to open socket");
	
		bzero(&d1, sizeof(d1));
		d1.sin_family = AF_INET;
		d1.sin_port = htons(join_port);
	
		if ( inet_aton(serv_addr, &d1.sin_addr.s_addr) == 0 )
			printf("error\n");
		sleep(3);
		//connect to chatroom
		if ( connect(room_sock, (struct sockaddr*)&d1, sizeof(d1)) != 0 ){
			printf("failed to connect to server\n");
		fflush(stdout);
		}
		//chatroom loop
		while(exit == 0){
			bzero(buf, 256);
			bzero(buffer, 256);
			FD_ZERO(&read_fds);
			FD_SET(room_sock, &read_fds);
			FD_SET(STDIN_FILENO, &read_fds);
			//memcpy(&time1, &time, sizeof(time));
	
			sel = select(FD_SETSIZE, &read_fds, NULL ,NULL, NULL);
			if (sel == -1) { 
			// error
        
			} 
		     else {
        
				// if stdin has activity, send the message
				if(FD_ISSET(STDIN_FILENO, &read_fds)){
					fgets(buffer, 256, stdin);
					m = write(room_sock, buffer, strlen(buffer));
				}
				// otherwise, display messages
				if(FD_ISSET(room_sock, &read_fds)){
					recv(room_sock, buf, 256, 0);
					printf("%s\n", buf);
				//check if chatroom closes
				if(strcmp("Chatroom shutting down", buf) == 0){
					close(room_sock);
					//exit to main loop
					exit = 1;
				}
					
				}
			}
	
		}
	}
	else
		close(sock);
}
    return 0;
}
