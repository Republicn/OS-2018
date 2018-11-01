#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

int all = 1;
char* dirin;
char signs[20];
int nums[20], links[20], sizes[20];
char* names[20];
char* paths[20];
int cntnames, cntpaths;

/*
void exec(char* prog) {
	char* args[2];
	strcpy(args[0], prog);
	strcpy(args[1], dirin);
	int status;
   	char* newx[] = { NULL };
   	pid_t pid  = fork();
   	if (pid == 0) {
   		execve(prog, args, newx);
   		free(args[0]);
   		free(args[1]);
   		perror("execve");
   	} else if (pid < 0) {
   		free(args[0]);
   		free(args[1]);
   		exit(EXIT_FAILURE);
   		perror("fork");
   	} else {
   		if (wait(&status) == -1) {
   			perror("wait");
   			free(args[0]);
   			free(args[1]);
   			exit(EXIT_FAILURE);
   		}
   		if (WIFEXITED(status))
   			printf("Child terminated normally with exit code %d\n", WEXITSTATUS(status));
   		if (WIFSIGNALED(status))
       		printf("Child was terminated by a signal #%d\n", WTERMSIG(status));
       	if (WCOREDUMP(status))
       		printf("Child dumped core\n");
       	if (WIFSTOPPED(status))
       		printf("Child was stopped by a signal #%d\n", WSTOPSIG(status));
   		free(args[0]);
   		free(args[1]);   
   	}
}
*/

void dfs(char* dir) {
	printf("start dfs %s\n", dir);
	struct dirent* dentry;
	struct stat st;
	printf("dir = >>>%s<<<\n", dir);
	DIR* d = opendir(dir);
	printf("open dir = >>>%s<<<\n", dir);
	while ((dentry = readdir(d)) != NULL) {
		if (strcmp(dentry->d_name, ".") == 0 || strcmp(dentry->d_name, "..") == 0) {
			continue;
		} 
		printf("cur file >>>%s<<<\n", dentry->d_name);

		char* path = (char*)malloc(strlen(dir) + strlen(dentry->d_name) + 1);

		strcat(path, dir);
		
		strcat(path, "/");
		strcat(path, dentry->d_name);
		
		if (all) {
			printf("%s\n", path); 
		}
		stat(path, &st);
		
		if (S_ISDIR(st.st_mode)) {
		    dfs(path);
		}
		
		free(path);
		
		
		/*
		if (S_ISDIR(st.st_mode)) {
		    char* path;
		    strcpy(path, strcat(dir, dentry->d_name));
		    dfs(path);
		    free(path);
		} else if (S_ISREG(st.st_mode)) {
			int fl = 1;
			if (nums[0] != 0) {
				for(int i = 1; i <= nums[0]; i++) 
					if (st.st_ino != nums[i]) {
						fl = 0;
						continue;
					}
			}
			if (names[0] != 0) {
				for(int i = 1; i <= cntnames; i++) 
					if (dentry->d_name != names[i]) {
						fl = 0;
						continue;
					} 
			}
			if (sizes[0] != 0) {
				for(int i = 1; i <= sizes[0]; i++) 
					if (st.st_size != sizes[i]) {
						fl = 0;
						continue;
					}
			}
			if (links[0] != 0) {
				for(int i = 1; i <= links[0]; i++) 
					if (st.st_nlink != links[i]) {
						fl = 0;
						continue;
					}
			}
			if (fl) {
				printf("%s", dir);
				if (cntpaths != 0) 
					for (int i = 1; i <= cntpaths; i++)
						exec(paths[i]);
			}				
		}*/
	}
	closedir(d);	
	printf("end dfs %s\n", dir);
	//free(d);
	
}

int main(int argc, char* argv[]) {
	if ((argc < 2) || (argc % 2 == 1))
		return EXIT_FAILURE;
	cntnames = cntpaths = 0;
	signs[0] = nums[0] = links[0] = sizes[0] = 0;
	
	//dirin = malloc(strlen(argv[1]));
	//strncpy(dirin, argv[1], strlen(argv[1]));
	if (argc > 2) {
		all = 0;
	}
	dirin = argv[1];
	int cnt = 1;
	    /*
	while (cnt != (argc - 1)) {
		cnt += 2;
		printf("%d %d\n", cnt, argc);
		printf("%s\n", argv[cnt - 1]); 

		if (strcmp(argv[cnt - 1], "-inum") == 0) {
  			nums[++nums[0]] = atoi(argv[cnt]);
  		} else if (strcmp("-name", argv[cnt - 1]) == 0) {
  			names[++cntnames] = argv[cnt];		
  	 	} else if (strcmp("-size", argv[cnt - 1]) == 0) {
  	 		signs[++signs[0]] = argv[cnt][0];
  	 		char* mas;
  	 		strncpy(mas, argv[cnt] + 1, sizeof(argv[cnt]) - 1);
  	 		sizes[++sizes[0]] = atoi(mas);
  	 		free(mas);
  	 	} else if (strcmp("-nlinks", argv[cnt - 1]) == 0) {
  	 		links[++links[0]] = atoi(argv[cnt]);
  	 	} else if (strcmp("-exec", argv[cnt - 1]) == 0) {
  	 		paths[++cntpaths] = argv[cnt];
  	 	} else {
  	 		printf("Usage: ./find.out <path> -<key> <arg>...");
  	 		return EXIT_FAILURE;
  	 	}
  	 	
  	}
  	    */
	//fflush(stdin);
  	dfs(dirin);
}