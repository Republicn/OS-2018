#include <string>
#include <fcntl.h>

using namespace std;

void checkError(int code, string message) {
	if (code < 0) {
		perror(message.c_str());
		exit(EXIT_FAILURE);
	}	
}
void checkErrno(string message) {
	if (errno != 0) {
		perror(message.c_str());
	}
} 

void setNonBlocking(int sock) {
	string err = (string)"flags for socket with fd " + to_string(sock);
	int cur_flags = fcntl(sock, F_GETFL);
	checkError(cur_flags, err);
	cur_flags = cur_flags | O_NONBLOCK; 
	checkError(fcntl(sock, F_SETFL, cur_flags), err);
}

void epollModEvents(int epoll_fd, int fd, uint32_t events) {
	struct epoll_event ev;
	ev.data.fd = fd;
	ev.events = events;
	checkError(epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &ev), "epoll_ctl_mod");
}