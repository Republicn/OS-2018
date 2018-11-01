#ifndef SOCKET_UTILS_H
#define SOCKET_UTILS_H

#include <sys/socket.h>
#include <string>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>

#define handle_error(eval, msg) \
            if(eval < 0) {perror(msg); exit(EXIT_FAILURE);}

#define error(msg) \
           do { perror(msg); exit(EXIT_FAILURE); } while (0)

#define BUF_SIZE 1024
#define EPOLL_SIZE 10000
#define EPOLL_RUN_TIMEOUT (3 * 60 * 1000)

ssize_t sendall(int s, void *buf, ssize_t len, int flags) {
    ssize_t total = 0;
    ssize_t n = -1;

    while (total < len) {
        n = send(s, buf + total, (size_t) (len - total), flags);
        if (n == -1) { break; }
        total += n;
    }

    return (n == -1 ? -1 : total);
}


ssize_t recvall(int s, char *buf, ssize_t len, int flags) {
    ssize_t total = 0;
    ssize_t n = -1;

    do {
        n = recv(s, buf + total, (size_t) (len - total), flags);
        if (n == -1) { break; }
        total += n;
        char *pch = strchr(buf, '\n');
        if (pch != nullptr) { break; }
    } while (n != 0 && n != -1);

    return (n == -1 ? -1 : total);
}


ssize_t writeall(int s, void *buf, ssize_t len) {
    ssize_t total = 0;
    ssize_t n = -1;

    while (total < len) {
        n = write(s, buf + total, (size_t) (len - total));
        if (n == -1) { break; }
        total += n;
    }

    return (n == -1 ? -1 : total);
}

ssize_t readall(int s, char *buf, ssize_t len) {
    ssize_t total = 0;
    ssize_t n = -1;

    do {
        n = read(s, buf + total, (size_t) (len - total));
        if (n == -1) { break; }
        total += n;
        char *pch = strchr(buf, '\n');
        if (pch != nullptr) { break; }
    } while (n != 0 && n != -1);

    return (n == -1 ? -1 : total);
}

// Setup nonblocking socket
int setnonblocking(int sockfd) {
    handle_error(fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0) | O_NONBLOCK), "fcntl");
    return 0;
}

#endif //SOCKET_UTILS_H
