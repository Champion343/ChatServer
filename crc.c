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

int main(int argc, char *argv[]) {

struct sockaddr_in d, d1;
int sock, room_sock;
int join_port, exit;
char buffer[256],  buf[256];
int port = 9645;
char *serv_addr = "127.0.0.1";
int m, n, sel, max_fd;
fd_set read_fds;

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
	
	bzero(buffer, 256);
	fgets(buffer,255,stdin);
	
    if ( connect(sock, (struct sockaddr*)&d, sizeof(d)) != 0 )
        printf("failed to connect to server\n");
	
    n = write(sock, buffer, strlen(buffer));
	
	bzero(buffer, 256);
	n = read(sock, buffer, 255);
	printf("%s\n", buffer);
    close(sock);
	if(buffer[0] == 'p'){
		char *temp_str;
		temp_str = &buffer[5];
			
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
		if ( connect(room_sock, (struct sockaddr*)&d1, sizeof(d1)) != 0 ){
			printf("failed to connect to server\n");
		fflush(stdout);
		}
		while(exit == 0){
			bzero(buf, 256);
			bzero(buffer, 256);
			FD_ZERO(&read_fds);
			FD_SET(room_sock, &read_fds);
			FD_SET(STDIN_FILENO, &read_fds);
			//memcpy(&time1, &time, sizeof(time));
	
			sel = select(FD_SETSIZE, &read_fds, NULL ,NULL, NULL);
			if (n == -1) { 
			// some error occurs
        
			} /*else if (n == 0) {
			// timeout, no activity
			printf("Timeout after 30 seconds.\n");
			printf("%s\n",buffer);
		}*/ else {
        
        // if stdin has activity, send the message
				if(FD_ISSET(STDIN_FILENO, &read_fds)){
					fgets(buffer, 256, stdin);
					m = write(room_sock, buffer, strlen(buffer));
				}
				// otherwise, display messages
				if(FD_ISSET(room_sock, &read_fds)){
					recv(room_sock, buf, 256, 0);
					printf("%s\n", buf);
				if(strcmp("Chatroom shutting down", buf) == 0){
					close(room_sock);
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
