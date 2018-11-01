#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <string>
#include <iostream>

using std::string;

char signs[20];
int nums[20], links[20]; 
long long sizes[20];
string names[20];
string paths[20];
int cnt_names, cnt_paths, cnt_nums, cnt_links, cnt_sizes;

void exec(string prog, string path) {
	char*  args[3] = {(char *)prog.c_str(), (char*)path.c_str(), NULL};
	int status;
   	char* newx[] = { NULL };
   	pid_t pid  = fork();
   	if (pid == 0) {
   		execve(args[0], args, newx);
   		perror("execve");
   	//	exit(EXIT_FAILURE);
   	} else if (pid < 0) {
   		perror("fork");
   //		exit(EXIT_FAILURE);
   	} else {
   		if (wait(&status) == -1) {
   			perror("wait");
   	//		exit(EXIT_FAILURE);
   		}
   /*		if (WIFEXITED(status))
   			printf("Child terminated normally with exit code %d\n", WEXITSTATUS(status));
   		if (WIFSIGNALED(status))
       		printf("Child was terminated by a signal #%d\n", WTERMSIG(status));
       	if (WCOREDUMP(status))
       		printf("Child dumped core\n");
       	if (WIFSTOPPED(status))
       		printf("Child was stopped by a signal #%d\n", WSTOPSIG(status)); */  
   	}
}

bool size_is_ok(char cmp, long long file_size, long long size) {
	switch (cmp) {
		case '=':
			return file_size == size;
		case '+':
			return file_size > size;
		case '-':
			return file_size < size;
	}
	return 0;
}

void quitt(string err, string name) {
	perror((err + " " + name).c_str());
	//exit(EXIT_FAILURE);
}


void dfs(string dir) {
	struct dirent* dentry;
	struct stat st;
	DIR* d = opendir(dir.c_str());
	if (d == NULL) {
		quitt("Cannot open directory", dir);
		return;
	}
	while ((dentry = readdir(d)) != NULL) {
		string file_name = string(dentry->d_name);
		if (file_name == "." || file_name == "..") {
			continue;
		} 
		string path = dir + "/" + file_name;
		if (stat(path.c_str(), &st) == -1) {
			quitt("Cannot get information about", path);
			continue;
		}
		
		if (S_ISDIR(st.st_mode)) {
		    dfs(path);
			continue;
		}
		if (S_ISREG(st.st_mode)) {
			int fl = 1;
			for (int i = 0; i < cnt_nums; i++) {
				if ((int)st.st_ino != nums[i]) {
					fl = 0;
					break;
				}
			}
			for (int i = 0; i < cnt_names; i++) 
				if (file_name != names[i]) {
					fl = 0;
					break;
				} 
			for (int i = 0; i < cnt_sizes; i++) 
				if (!size_is_ok(signs[i], st.st_size, sizes[i])) {
					fl = 0;
					break;
				}
			for (int i = 0; i < cnt_links; i++) 
				if ((int)st.st_nlink != links[i]) {
					fl = 0;
					break;
				}
			
			if (fl) {
				if (cnt_paths == 0)
					printf("%s\n", path.c_str());
				else
					for (int i = 0; i < cnt_paths; i++)
						exec(paths[i], path);
			}				
		}

		
	}
	if (errno != 0) {
		quitt("Cannot read from directory", dir);
	}
	if (closedir(d) == -1) {
		quitt("Cannot close directory", dir);
	}	
}

int main(int argc, char* argv[]) {
	if ((argc < 2) || (argc % 2 == 1)) {
		printf("Usage: ./find <path> -<key> <arg>...\n");
		return 0;
	}
	string dirin = string(argv[1]);
	int cnt = 1;
	while (cnt != (argc - 1)) {
		cnt += 2;
		string arg(argv[cnt - 1]);
		if (arg == "-inum") {
  			nums[cnt_nums++] = atoi(argv[cnt]);
  		} else if (arg == "-name") {
  			names[cnt_names++] = argv[cnt];		
  	 	} else if (arg == "-size") {
  	 		signs[cnt_sizes] = argv[cnt][0];
  	 		sizes[cnt_sizes++] = atoll(string(argv[cnt]).substr(1).c_str());
  	 	} else if (arg == "-nlinks") {
  	 		links[cnt_links++] = atoi(argv[cnt]);
  	 	} else if (arg == "-exec") {
  	 		paths[cnt_paths++] = argv[cnt];
  	 	} else {
  	 		printf("Incorrect argument\n");
  	 		return 0;
  	 	}
  	 	
  	}
  	dfs(dirin);
}