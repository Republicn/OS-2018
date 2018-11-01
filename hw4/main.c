#include <stdio.h>
#include <dlfcn.h>
#include <unistd.h>
#include <string.h>

extern int f1();
extern void f2();
extern void f3();
extern int f4();

const size_t MAX_DIR = 4096;

int main() {
	f2();
	printf("The result is %d\n", f1());
	
	f3();
	printf("The result is %d\n", f4());
	
	void* lib;
	int (*func1)(void);
	void (*func2)(void);
	char dir[MAX_DIR];
	if (!getcwd(dir, MAX_DIR)) {
		perror("getcwd:");
		return 1;
	}
	char* lib_name = "/libdynamic2.so";
	char* path_to_lib = strcat(dir, lib_name); 
	lib = dlopen(path_to_lib, RTLD_LAZY);
	if (!lib) {
		printf("dlopen() error: %s\n", dlerror());
		return 1;
	}
	func2 = dlsym(lib, "f6");
	func1 = dlsym(lib, "f5");
	func2();
	printf("The result is %d\n", func1());
	dlclose(lib);
}