#include <cstdio>
#include <cstdlib>
#include <sys/mman.h>
#include <string.h>

char mcode[] = {0x55, 0x48, (char)0x89, (char)0xe5, (char)0xb8, (char)0xa1, 0x00, 0x00, 0x00, 0x5d, (char)0xc3};

void write_int(int pos, int c) {
	for (int i = 0; i < 4; i++) {
		mcode[pos + i] = c & 0xff;
		c >>= 8;
	}
}

int main(int argc, char* argv[]) {
	if (argc < 2) {
		printf("Usage: sumjit a b\n return a + b\n");
		return 0;
	}
	
	int arg1 = atoi(argv[1]), arg2 = atoi(argv[2]);
	write_int(5, arg1 + arg2);

	size_t length = sizeof(mcode);
	void* buf = mmap(NULL, length, PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
	if (buf == MAP_FAILED) {
		perror("mmap");
	}
	memcpy(buf, mcode, length);
	if (mprotect(buf, length, PROT_EXEC | PROT_READ)) {
		perror("mprotect");
	}
	int res = ((int (*)(void)) buf)();
    if (munmap(buf, length)) {
        perror("munmap");
    };
    printf("The result of given function is %d\n", res);
    return 0;
}                