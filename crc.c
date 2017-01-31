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

struct sockaddr_in d;
int sock, room_sock;
int join_port = 0;
char buffer[256],  buf[256];
int port = 9650;
char *serv_addr = "127.0.0.1";
int m, n, sel, max_fd;
fd_set read_fds;
struct timeval time, time1;

	time.tv_sec = 30;
	time.tv_usec = 0;
	
	for(;;){
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
	if(buffer[0] == "p"){
		char *temp_str;
		temp_str = &buffer[5];
			
		join_port = atoi(temp_str);
	printf(join_port);
	fflush(stdout);
	d.sin_port = htons(join_port);
    if ( inet_aton(serv_addr, &d.sin_addr.s_addr) == 0 )
if ( connect(sock, (struct sockaddr*)&d, sizeof(d)) != 0 )
	printf("failed to connect\n");
	for(;;){
	printf("we did it\n");
	fflush(stdout);
	bzero(buf, 256);
	FD_ZERO(&read_fds);
	FD_SET(sock, &read_fds);
	FD_SET(STDIN_FILENO, &read_fds);
	//memcpy(&time1, &time, sizeof(time));
	time.tv_sec = 30;
	time.tv_usec = 0;
	
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
			m = write(sock, buffer, strlen(buffer));
			bzero(buffer, 256);
		}
        // otherwise, display messages
		if(FD_ISSET(sock, &read_fds)){
			recv(sock, buf, 256, 0);
			printf("%s\n", buf);
		}
    }
	
	}
	close(sock);
	/*
    bzero(buffer, 256);
	fgets(buffer,255,stdin);
    n = write(sock, buffer, strlen(buffer));
	bzero(buffer, 256);
	n = read(sock, buffer, 255);
	printf("%s\n", buffer);
    close(sock);
		if(buffer[0] == 9 && isdigit(*buffer)){
		int join_port = strtol(buffer, &buffer, 10);
		printf(join_port);
		fflush(stdout);
	d.sin_port = htons(join_port);
    if ( inet_aton(serv_addr, &d.sin_addr.s_addr) == 0 )
    {
        printf("error\n");
    }
	}
	bzero(buffer, 256);
	printf("To send another message to the server type 'y' otherwise hit any key...");
	//yes_no = getchar();*/
	}
}
    return 0;
}
