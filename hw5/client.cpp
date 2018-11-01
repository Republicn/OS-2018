#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <string.h>

using namespace std;

char question[] = "What time is it now?";

int main(int argc, char* argv[]) {
	if (argc < 2) {
		printf("Usage: ./client <port> [<address>]\n");
		return 0;
	}
	int port_num = atoi(argv[1]);
	char buf[9];
	int client;
	if ((client = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("client socket");
		return 1;
	}	
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port_num);
	addr.sin_addr.s_addr = argc < 3 ? htonl(INADDR_LOOPBACK) : inet_addr(argv[2]);

	if (connect(client, (struct sockaddr*) &addr, sizeof(addr)) < 0) {
    	perror("connect");
    	return 1;
	}
	int rem = sizeof(question);
	int pos = 0;
	while (rem != 0) {
		int cnt = send(client, question + pos, rem, 0);
    	if (cnt == -1) {
    		perror("send");
    		return 1;
    	}
    	rem -= cnt;
	} 
	int len = 0;
	char tmp[10];
	while (len < 9) {
		int num_bytes = recv(client, tmp, 9, 0);
		if (num_bytes < 0) {
			break;
		}
		memcpy(buf + len, tmp, num_bytes);
		len += num_bytes;
	}
	if (errno != 0) {
		perror("recv");
		return 1;
	}
	printf("Current time: %s\n", buf);
	close(client);
	if (errno != 0) {
   		perror("close client");
   		return 1;
   	}	
	return 0;
}