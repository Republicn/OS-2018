#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdio>
#include <cstdlib>
//#include <zconf.h>
#include <iostream>
#include <stdint-gcc.h>
#include <sys/epoll.h>
#include "utils.h"

int main(int argc, char *argv[]) {
    if (argc == 1) {
        std::cout << "Wrong formant. Please enter port" << std::endl;
        return 1;
    }

    char *err;
    auto port = (uint16_t) strtol(argv[1], &err, 0);


    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        error("socket");
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    if (connect(sock, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        error("connect");
    }

    int pipe_fd[2];
    handle_error(pipe(pipe_fd), "pipe");

    int epfd = epoll_create(EPOLL_SIZE);
    if (epfd < 0) {
        error("epoll_create");
    }

    static struct epoll_event ev, events[2]; // Socket(in|out) & Pipe(in)
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = sock;
    // add server connetion(sock) to epoll to listen incoming messages from server
    handle_error(epoll_ctl(epfd, EPOLL_CTL_ADD, sock, &ev), "epoll_ctl");

    // add read part of pipe(pipe_fd[0]) to epoll
    // to listen incoming messages from child process
    ev.data.fd = pipe_fd[0];
    handle_error(epoll_ctl(epfd, EPOLL_CTL_ADD, pipe_fd[0], &ev), "epoll_ctl");

    pid_t pid = fork();
    if (pid < 0) {
        error("fork");
    }

    if (pid == 0) {
        close(pipe_fd[0]);
        std::cout << "Enter 'exit' to exit" << std::endl;
        while (true) {
            std::string s;
            getline(std::cin, s);
            if (std::cin.rdstate() != std::ios::goodbit) {
                exit(EXIT_FAILURE);
            }
            if (s == "exit") {
                break;
            }
            s += '\n';
            writeall(pipe_fd[1], (void *) s.c_str(), s.length());
        }
    } else {
        close(pipe_fd[1]);
        char message[BUF_SIZE];
        bool continue_work = true;
        while (continue_work) {
            int epoll_events_count = epoll_wait(epfd, events, 2, EPOLL_RUN_TIMEOUT);
            if (epoll_events_count < 0) {
                error("epoll_wait");
            }
            for (int i = 0; i < epoll_events_count; i++) {
                bzero(&message, BUF_SIZE);
                // new message from server
                if (events[i].data.fd == sock) {
                    std::cout << "New message from server" << std::endl;
                    ssize_t read = recvall(sock, message, BUF_SIZE, 0);
                    if (read == 0) {
                        std::cout << "Server closed connection" << std::endl;
                        close(sock);
                        continue_work = false;
                        break;
                    }

                    if (read < 0) {
                        if (errno != 0) {
                            perror("recv");
                        }
                        continue;
                    }

                    int hash = atoi(message);

                    std::cout << " [" << std::hex << hash << "]" << std::endl;
                } else { // user's input message
                    ssize_t r = readall(events[i].data.fd, message, BUF_SIZE);
                    if (r == 0) {
                        continue_work = false;
                        break;
                    }
                    handle_error(r, "recv");
                    handle_error(sendall(sock, message, strlen(message), 0), "send");
                }
            }

        }
    }


    if (pid) {
        close(pipe_fd[0]);
        close(sock);
    } else {
        close(pipe_fd[1]);
    }

    return 0;
}
