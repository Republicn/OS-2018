#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <cstdio>
#include <cstdlib>
//#include <zconf.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <cstring>
#include <algorithm>
#include "utils.h"


int hash(const char *s, ssize_t len) {
    int h = 0;
    for (int i = 0; i < len; i++) {
        h = 31 * h + s[i];
    }
    return h;
}

std::vector<int> clients;

int main(int argc, char *argv[]) {
    if (argc == 1) {
        std::cout << "Wrong formant. Please enter port" << std::endl;
        return 1;
    }

    char *err;
    auto port = (uint16_t) strtol(argv[1], &err, 0);

    int listener;

    listener = socket(AF_INET, SOCK_STREAM, 0);
    if (listener < 0) {
        error("socket");
    }

    setnonblocking(listener);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    handle_error(bind(listener, (struct sockaddr *) &addr, sizeof(addr)), "bind");

    handle_error(listen(listener, 1), "listen");

    int epfd = epoll_create(EPOLL_SIZE);
    if (epfd < 0) {
        error("epoll_create");
    }

    static struct epoll_event ev, events[EPOLL_SIZE];
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = listener;

    //add listener to epoll
    handle_error(epoll_ctl(epfd, EPOLL_CTL_ADD, listener, &ev), "epoll_ctl");

    std::cout << "Server started on port " << port << std::endl;

    char buf[BUF_SIZE];

    while (true) {
        int epoll_events_count = epoll_wait(epfd, events, EPOLL_SIZE, EPOLL_RUN_TIMEOUT);
        if (epoll_events_count < 0) {
            error("epoll_wait");
        }

        for (int i = 0; i < epoll_events_count; i++) {
            // new client connection
            if (events[i].data.fd == listener) {
                int client = accept(listener, nullptr, nullptr);
                setnonblocking(client);
                ev.data.fd = client;

                // add new client to epoll
                handle_error(epoll_ctl(epfd, EPOLL_CTL_ADD, client, &ev), "epoll_ctl");
                clients.push_back(client);

                std::cout << "<< client connected " << client << std::endl;
            } else { // new request
                int client = events[i].data.fd;
                ssize_t read = recvall(client, buf, BUF_SIZE, 0);
                if (read == 0) {
                    close(client);
                    clients.erase(std::find(clients.begin(), clients.end(), client));
                    std::cout << ">> client disconnected " << client << std::endl;
                    continue;
                }
                if (read < 0) {
                    if (errno != 0) {
                        perror("recv");
                    }
                    continue;
                }
                ssize_t len = read - 1;
                buf[len] = '\0';
                std::cout << "received from " << client << " '" << buf << "'" << std::endl;

                int h = hash(buf, len);
                std::string msg = std::to_string(h) + "\n";
                ssize_t scode = sendall(client, (void *) msg.c_str(), msg.size(), 0);
                if (scode <= 0) {
                    if (errno != 0) {
                        perror("send");
                    }
                    continue;
                }
            }
        }
    }

    close(listener);
    close(epfd);

    return 0;
}