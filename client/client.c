#include <netdb.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#define bufmax 100

int main(int argc, char *argv[]) {

struct sockaddr_in d;
int sock;
char buffer[256];
int port = 9114;
char *serv_addr = "127.0.0.1";
int n;

sock = socket(AF_INET, SOCK_STREAM, 0);
if(sock < 0)
	printf("unable to open socket");

    bzero(&d, sizeof(d));
    d.sin_family = AF_INET;
    d.sin_port = htons(port);
    if ( inet_aton(serv_addr, &d.sin_addr.s_addr) == 0 )
    {
        printf("error\n");
    }

    if ( connect(sock, (struct sockaddr*)&d, sizeof(d)) != 0 )
    {
        printf("failed to connect\n");
    }
	/*printf("what the what");
	fflush(stdout);
	*/
    bzero(buffer, 256);
	fgets(buffer,255,stdin);
    n = write(sock, buffer, strlen(buffer));

    close(sock);
    return 0;
}