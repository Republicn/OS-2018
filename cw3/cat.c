#include <stdio.h>
#include <unistd.h>

int main(int argc, const char* argv[]) {
	while (1) {
		if (argc != 2) {
			printf("Invalid number of arguments");
			return EXIT_FAILURE; 
		}
		int deskr = open(argv[1]);
		char* path;
		size_t buf = 1024;
		while (read(deskr, path, buf) != )
	}
	return 0;
}
