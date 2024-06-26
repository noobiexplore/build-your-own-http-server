#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

int main() {
	// Disable output buffering
	setbuf(stdout, NULL);

	// You can use print statements as follows for debugging, they'll be visible when running tests.
	printf("Logs from your program will appear here!\n");

	// Uncomment this block to pass the first stage
	//
	int server_fd; 
	struct sockaddr_in client_addr;
  socklen_t client_addr_len;
	
	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd == -1) {
		printf("Socket creation failed: %s...\n", strerror(errno));
		return 1;
	}
	
	// Since the tester restarts your program quite often, setting REUSE_PORT
	// ensures that we don't run into 'Address already in use' errors
	int reuse = 1;
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(reuse)) < 0) {
		printf("SO_REUSEPORT failed: %s \n", strerror(errno));
		return 1;
	}
	
	struct sockaddr_in serv_addr = { .sin_family = AF_INET ,
									 .sin_port = htons(4221),
									 .sin_addr = { htonl(INADDR_ANY) },
									};
	
	if (bind(server_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) != 0) {
		printf("Bind failed: %s \n", strerror(errno));
		return 1;
	}
	
	int connection_backlog = 5;
	if (listen(server_fd, connection_backlog) != 0) {
		printf("Listen failed: %s \n", strerror(errno));
		return 1;
	}
	
	printf("Waiting for a client to connect...\n");
	client_addr_len = sizeof(client_addr);
	
	int client_fd = accept(server_fd, (struct sockaddr *) &client_addr, &client_addr_len);
  if (client_fd == -1) {
    printf("Client connection failed: %s...\n", strerror(errno));
    return 1;
  }
	printf("Client connected\n");

  // Recieve data from the client
  char *recv_buffer;
  int max_buffer_len = 100;
  int bytes_recieved = recv(client_fd, recv_buffer, max_buffer_len, 0);
  if (bytes_recieved == -1) {
    printf("Recieving data failed: %s...\n", strerror(errno));
  }

  // Response to the client
  char *response = "HTTP/1.1 200 OK\r\n\r\n";
  int response_len = strlen(response);
  int bytes_sent = send(client_fd, response, response_len, 0);
  if (bytes_sent == -1) {
    printf("Sending response failed: %s\n", strerror(errno));
  }
	
	close(server_fd);

	return 0;
}
