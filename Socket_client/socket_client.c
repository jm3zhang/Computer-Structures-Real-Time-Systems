#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

int main(void) {
	/* open socket */
	int sockfd = socket(AF_INET6, SOCK_STREAM, 0);
	if(sockfd < 0) { printf("error opening socket"); return 0; }

	/* request connection */
	struct hostent *he = gethostbyname("localhost");
	if(he == NULL) { printf("hostname not found"); return 0; }
	struct sockaddr_in server_addr = { AF_INET6, htons(1101), 0 };
	memcpy(&server_addr.sin_addr, he->h_addr_list[0], he->h_length);
	if(connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr))
			< 0)
	{	printf("error requesting connection\n"); return 0; }

	/* send and receive messages */
	char request[] = "request";
	int n = write(sockfd, request, sizeof(request));
	if(n < 0) { printf("error writing message"); return 0; }
	uint8_t response[256];
	n = read(sockfd, response, sizeof(response));
	if(n < 0) { printf("error reading message"); return 0; }
	printf("client received: %s\n", response);
}
