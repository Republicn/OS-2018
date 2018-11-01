#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <string>
#include <fcntl.h>
#include <sys/epoll.h>
#include "pieceOfShit.h"  
#include <map>
#include <vector>

using namespace std;

char buf[100];

char qw[] = "What time is it now?";

int receive(int new_socket) {
    int len = 0;
    char tmp[30];
    while (len < (int)sizeof(qw)) {
        int num_bytes = recv(new_socket, tmp, 3, 0);
        if (errno != 0) {
        	perror("recv");
        	return -1;
        }
        if (num_bytes < 0) {
        	break;
        } else if (num_bytes == 0) {
        	close(new_socket);
        	return -1;
        }           
        memcpy(buf + len, tmp, num_bytes);
        len += num_bytes;
    }	
    return 0;
}

void sending(int new_socket) {
	printf("%s\n", buf);
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
        	checkError(cnt, "send"); 
        	rem -= cnt;
        }
    }
}

int main(int argc, char* argv[]) {
	if (argc < 2) {
	    printf("Usage: ./server <port>\n");
		return 0;
	}
   	int port_num = atoi(argv[1]);
   	int listener;
   	checkError(listener = socket(AF_INET, SOCK_STREAM, 0), "server socket");
   	setNonBlocking(listener);	
   	struct sockaddr_in addr;
   	addr.sin_family = AF_INET;
   	addr.sin_port = htons(port_num);
   	addr.sin_addr.s_addr = htonl(INADDR_ANY);
   	checkError(bind(listener, (struct sockaddr*) &addr, sizeof(addr)), "bind");
   	checkError(listen(listener, 1000), "listen");

   	int epollFD = epoll_create(1000);
   	if (epollFD == -1) {
   		perror("epoll_create");
  		return EXIT_FAILURE;
   	}
   	struct epoll_event ev, events[1000];
   	ev.events = EPOLLET | EPOLLIN;
	ev.data.fd = listener;
	checkError(epoll_ctl(epollFD, EPOLL_CTL_ADD, listener, &ev), "epoll_ctl: listener");
    map<int, vector<string>> requests;
    while (true) {
        int new_socket, num_events;
        
        checkError(num_events = epoll_wait(epollFD, events, 1000, 100000), "epoll_wait");
        for (int i = 0; i < num_events; i++) {
        	int curFD = events[i].data.fd;
        	if (curFD == listener) {
        		cout << "New client connected" << endl;
        		checkError(new_socket = accept(listener, NULL, NULL), "accept to a new socket");
                setNonBlocking(new_socket);
                ev.data.fd = new_socket;
                ev.events = EPOLLIN | EPOLLET;
                checkError(epoll_ctl(epollFD, EPOLL_CTL_ADD, new_socket, &ev), "epoll_ctl");
        	} else if (events[i].events & EPOLLIN) {
				if (receive(curFD) == -1) {
				    cout << "Client disconnected" << endl;
					continue;
				}
				cout << "Client send request" << endl;

				if (requests.count(curFD) == 0) {
					requests[curFD] = vector<string>();
					epollModEvents(epollFD, curFD, EPOLLIN | EPOLLOUT);
				}
				requests[events[i].data.fd].push_back(buf);
        	} else if (events[i].events & EPOLLOUT) {
        		cout << "Sending answer to a client" << endl;
        		sending(curFD);
        		requests[curFD].pop_back();
        		if (requests[curFD].empty()) {
					requests.erase(curFD);
					epollModEvents(epollFD, curFD, EPOLLIN);
                }
        	}
        }
   	}
   	close(epollFD);
   	checkErrno("close epoll");
   	close(listener);
   	checkErrno("close listener");
	return 0;
}