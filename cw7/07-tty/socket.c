#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>

int log_syscall(int status, const char * msg)
{
    perror(msg);
    if (status == -1)
        _exit(EXIT_FAILURE);
    return status;
}

#define log(x) log_syscall(x, #x)

void make_daemon()
{
    pid_t pid = log(fork());
    if (pid)
        _exit(EXIT_SUCCESS);
    log(setsid());
    pid = log(fork());
    if (pid)
        _exit(EXIT_SUCCESS);
}

int create_socket(const char * service, int backlog)
{
    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_socktype = SOCK_STREAM;
    struct addrinfo * addrinfo;
    if (getaddrinfo(NULL, service, &hints, &addrinfo) != 0)
        printf("getaddrinfo() failed\n"),
        _exit(EXIT_FAILURE);
    else
        printf("getadderinfo(): Success\n");

    int sockfd = log(socket(addrinfo->ai_family,
                              addrinfo->ai_socktype,
                              addrinfo->ai_protocol));

    int reuse_address = 1;
    log(setsockopt(sockfd, SOL_SOCKET,
                   SO_REUSEADDR, (void *) &reuse_address, sizeof(reuse_address)));
    log(bind(sockfd, addrinfo->ai_addr, addrinfo->ai_addrlen));
    log(listen(sockfd, backlog));
    return sockfd;
}

int main(int argc, char ** argv)
{
    if (argc != 3)
        printf("Usage: %s <service> <backlog>\n", argv[0]),
        _exit(EXIT_FAILURE);

    make_daemon();
    int sockfd = create_socket(argv[1], atoi(argv[2]));
    while (1)
    {
        int accepted = log(accept(sockfd, NULL, NULL));
        const char * msg = "Hello, connection\n";
        write(accepted, msg, strlen(msg));
        close(accepted);
    }
}
