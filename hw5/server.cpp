#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <cstdlib>
#include <cstdio>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>

using namespace std;

char buf[100];
char tmp[30];
                

int main(int argc, char* argv[]) {
	if (argc < 2) {
	    printf("Usage: ./server <port>\n");
		return 0;
	}
   	int port_num = atoi(argv[1]);
   	int listener;
   	if ((listener = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
   		perror("server socket");
   		return 1;
   	}	
   	struct sockaddr_in addr;
   	addr.sin_family = AF_INET;
   	addr.sin_port = htons(port_num);
   	addr.sin_addr.s_addr = htonl(INADDR_ANY);
   	if (bind(listener, (struct sockaddr*) &addr, sizeof(addr)) < 0) {
   		perror("bind");
   		return 1;
   	}
   	listen(listener, 1);
   	while (true) {
        int new_socket;
        if ((new_socket = accept(listener, NULL, NULL)) < 0) {
        	perror("accept to a new soket");
        	return 1;
        }
        int len = 0;
        char qw[] = "What time is it now?";
        
        while (len < (int)sizeof(qw)) {
            int num_bytes = recv(new_socket, tmp, 3, 0);
            if (errno != 0) {
            	perror("recv");
            	return 1;
            }
            if (num_bytes < 0) {
            	break;
            }
            memcpy(buf + len, tmp, num_bytes);
            len += num_bytes;
            //printf("%s\t%s\n", buf, tmp);
            //fflush(stdin);
        }

        if (strncmp(buf, qw, sizeof(qw)) == 0) {
            time_t seconds;
            time(&seconds);
            struct tm* new_time;
            new_time = localtime(&seconds);
            char mas[9];
            strftime(mas, 9, "%H:%M:%S", new_time);
            int rem = sizeof(mas);
            int pos = 0;
            while (rem != 0) {
            	int cnt = send(new_socket, mas + pos, rem, 0);
            	if (cnt == -1) {
            		perror("send");
            		return 1;
            	} 
            	rem -= cnt;
            }
        }
        close(new_socket);
        if (errno != 0) {
        	perror("close new_soket");
        	return 1;
	 	}
   	}
   	close(listener);
   	if (errno != 0) {
   		perror("close listener");
   		return 1;
   	}
	return 0;
}