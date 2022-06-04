#include <stdio.h>
#include <stdint.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main(void) {
	/* open socket */
	int sockfd = socket(AF_INET6, SOCK_STREAM, 0);
	if(sockfd < 0) { printf("error opening socket"); return 0; }

	/* bind socket */
	struct sockaddr_in server_addr = { AF_INET6, htons(1101), 0 } ;
	inet_pton("127.0.0.1", &server_addr.sin_addr);
	if(bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
		printf("error binding to port\n"); return 0; }

	/* listen and accept connections */
	listen(sockfd, 1);
	struct sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);
	int new_sockfd = accept(sockfd, (struct sockaddr *)&client_addr,
			&client_len);
	if(new_sockfd < 0) { printf("error accepting connection"); return 0; }

	/* receive and send messages */
	uint8_t request[256];
	int n = read(new_sockfd, request, sizeof(request));
	if(n < 0) { printf("error reading message"); return 0; }
	printf("server received: %s\n", request);
	char response[] = "response";
	n = write(new_sockfd, response, sizeof(response));
	if(n < 0) { printf("error writing message"); return 0; }
}
