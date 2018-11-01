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
int pipe_fd[2];
int epollFD;

void closing(int pid, int client) {
	if (pid == 0) {
        close(pipe_fd[1]);
        checkErrno("close pipe");
        return;
    }
    close(epollFD);
    checkErrno("close epollFD");
    close(pipe_fd[0]);
    checkErrno("close pipe");
    close(client);
    checkErrno("close client");
    exit(0);
}

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

	checkError(pipe(pipe_fd), "pipe");
	ev.events = EPOLLIN | EPOLLOUT; 
    ev.data.fd = pipe_fd[0];
    checkError(epoll_ctl(epollFD, EPOLL_CTL_ADD, pipe_fd[0], &ev), "epoll_ctl");

    pid_t pid = fork();
    vector<string> requests;         	
    checkError(pid, "fork");
    if (pid == 0) {
    	close(pipe_fd[0]);
        while (true) {                       
            string str;
            getline(cin, str);
            if (str == "go") {
            	int cnt = write(pipe_fd[1], "1", 1);
            	while (cnt < 1) {
            	    checkError(cnt, "write");
            		cnt = write(pipe_fd[1], "1", 1);
            	}
            } else if (str == "stop") {
            	closing(pid, client);
            	return 0;	
            } else {
            	printf("Incorrect action. Try 'go' to new current time, 'stop' to stop the program execution\n");
            }
            
        }
    } else {
    	close(pipe_fd[1]);
    	bool isClosed = false;
    	int counter = 0;
    	while (true) {
    		int num_events;
    		checkError(num_events = epoll_wait(epollFD, events, 2, 1000), "epoll_wait");
    		for (int i = 0; i < num_events; i++) {
    			if (events[i].data.fd == client) {
                	int len = 0;
                	char tmp[10];
                	while (len < 9) {
                		int num_bytes = recv(client, tmp, 9, 0);
                		if (num_bytes < 0) {
                			break;
                		} else if (num_bytes == 0) {
                			printf("server disconnected\n");
                			closing(pid, client);
                			return 0;
                		} 
                		memcpy(buf + len, tmp, num_bytes);
                		len += num_bytes;
                	}
                	if (errno != 0) {
                		perror("recv");
                	}
                	counter--;
                	printf("Current time: %s\n", buf);   			
    			} else {
    				char bf[200];
            		int cnt = read(events[i].data.fd, bf, 1);
              	    checkError(cnt, "read");
              		if (cnt == 0) {
              			isClosed = true;
                        break;
              		}
              		bf[cnt] = 0;
                	if ((string)bf == "1") {
                    	int rem = sizeof(question);
                    	int pos = 0;
                    	while (rem != 0) {
                    		int cnt = send(client, question + pos, rem, 0);
                        	checkError(cnt, "send");
                        	rem -= cnt;
                    	}
                    	counter++;
                	}    					
    			}
    		}
    		if (isClosed && !counter) {
			    closing(pid, client);
    			break;
    		}    		
    	}
    }                    
}       