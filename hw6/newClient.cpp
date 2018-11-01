#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/epoll.h>
#include "pieceOfShit.h"  

using namespace std;

char question[] = "What time is it now?";
int epollFD;

int main(int argc, char* argv[]) {
	if (argc < 2) {
		printf("Usage: ./client <port> [<address>]\n");
		return 0;
	}
	int port_num = atoi(argv[1]);
	char buf[9];
	
	int client;
	checkError(client = socket(AF_INET, SOCK_STREAM, 0), "client socket");
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port_num);
	addr.sin_addr.s_addr = argc < 3 ? htonl(INADDR_LOOPBACK) : inet_addr(argv[2]);
	
	checkError(connect(client, (struct sockaddr*) &addr, sizeof(addr)), "connect");
	checkError(epollFD = epoll_create(100000), "epoll_create");
    static struct epoll_event ev, events[2];
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = client;
    checkError(epoll_ctl(epollFD, EPOLL_CTL_ADD, client, &ev), "epoll_ctl");
    ev.events = EPOLLIN;
    ev.data.fd = STDIN_FILENO;
    checkError(epoll_ctl(epollFD, EPOLL_CTL_ADD, STDIN_FILENO, &ev), "epoll_ctl");

   	bool isClosed = false;
   	int counter = 0, counterGot = 0;
   	while (true) {
   		int num_events;
   		checkError(num_events = epoll_wait(epollFD, events, 2, 1000), "epoll_wait");
   		for (int i = 0; i < num_events; i++) {
   			if ((events[i].data.fd == client) && (events[i].events & EPOLLIN)) {
               	int len = 0;
               	char tmp[10];
               	while (len < 9) {
               		int num_bytes = recv(client, tmp, 9, 0);
               		if (num_bytes < 0) {
               			break;
               		} else if (num_bytes == 0) {
               			printf("server disconnected\n");
               			close(client);
            		    checkErrno("client");
            		    close(epollFD);
            		    checkErrno("epollFD");
               			return 0;
               		} 
               		memcpy(buf + len, tmp, num_bytes);
               		len += num_bytes;
               	}
               	if (errno != 0) {
               		perror("recv");
               	}
               	printf("Current time: %s\n", buf);
               	counterGot--;		
   			} else if ((events[i].data.fd == client) && (events[i].events & EPOLLOUT)) {
                int rem = sizeof(question);
                int pos = 0;
                while (rem != 0) {
                   	int cnt = send(client, question + pos, rem, 0);
                    checkError(cnt, "send");
                    rem -= cnt;
            	}
            	counter--;
            	if (!counter) {
            		epollModEvents(epollFD, client, EPOLLIN);
            	}
   			} else {
				string str;
           		getline(cin, str);
         		if (str == "go") {
         		    if (!counter) {
         		    	epollModEvents(epollFD, client, EPOLLOUT);
         		    }
          		 	counter++;
          		 	counterGot++;
          		} else if (str == "stop") {
           			isClosed = true;
          		} else {
           			printf("Incorrect action. Try 'go' to new current time, 'stop' to stop the program execution\n");
           		}
   			}
   		}
   		if (isClosed && !counter) {
   		    close(client);
   		    checkErrno("client");
   		    close(epollFD);
   		    checkErrno("epollFD");
   			break;
   		}    		
    }                    
}       