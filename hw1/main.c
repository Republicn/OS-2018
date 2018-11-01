#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>

int cnt;
char* args[10];

void quit() {
	for (int i = 0; i <= cnt; i++) {
    	free(args[i]);
    }
    exit(EXIT_FAILURE);
}

int main(int argc, const char* argv[]) {
	while (1) {
     	char str[250];
     	cnt = 0;                
    	int symb = 0, sc = 0;
    	if ((sc = scanf("%s", str)) == EOF) {
    		break;
    	} else if (sc == -1) {
    		printf("Error while reading\n");
    		quit();
    	} else {
    		args[cnt] = (char*)malloc(strlen(str));
    		
    		if (args[cnt] == NULL) {
    			printf("Error while memory allocation");
    			quit();
    		} 
    		strncpy(args[cnt], str, strlen(str));
    		
    		cnt++;
    	}
    	while ((symb = getchar()) == ' ') {
    		if (scanf("%s", str) == -1) {
    			printf("Error while reading\n");
    			quit();
    		} 
    		args[cnt] = (char*)malloc(strlen(str));
    		if (args[cnt] == NULL) {
    			printf("Error while memory allocation");
    			quit();
    		} 
    		strncpy(args[cnt], str, strlen(str));
    		cnt++;
    	}
        args[cnt] = NULL;
    	int status;
    	char* newx[] = { NULL };
    	pid_t pid  = fork();
    	if (pid == 0) {
    		execve(args[0], args, newx);
    		perror("execve");
    		quit();
    	} else if (pid < 0) {
    		perror("fork");
    		quit();
    	} else {
    		if (wait(&status) == -1) {
    			perror("wait");
    			return EXIT_FAILURE;
    		}
    		if (WIFEXITED(status))
    			printf("Child terminated normally with exit code %d\n", WEXITSTATUS(status));
    		if (WIFSIGNALED(status))
        		printf("Child was terminated by a signal #%d\n", WTERMSIG(status));
        	if (WCOREDUMP(status))
        		printf("Child dumped core\n");
        	if (WIFSTOPPED(status))
        		printf("Child was stopped by a signal #%d\n", WSTOPSIG(status));
    		
    		for (int i = 0; i <= cnt; i++) {
    			free(args[i]);
    		}
    
    	}
    	
    	if (symb == EOF) {
    		break;
    	}
	}
	return 0;
}
