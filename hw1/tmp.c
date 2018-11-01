#include <unistd.h>
#include <stdio.h>
 
int main (void) {
 
        pid_t p;
 
        printf("Original program,  pid=%d\n",  getpid());
        p = fork();
        if (p == 0) {
                printf("In child process,  pid=%d,  ppid=%d\n", 
                        getpid(),  getppid());
                for (int i = 0; i < 10; i++) {
                	printf("sleep %d\n", i);
                	fflush(stdout);
                	sleep(1);
                }
        } else {
        		waitpid(p);
                printf("In parent,  pid=%d,  fork returned=%d\n", 
                        getpid(),  p);

        }
}