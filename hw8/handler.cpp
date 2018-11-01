#include <signal.h>
#include <bits/siginfo.h>
#include <bits/sigset.h>
#include <algorithm>
#include <sys/ucontext.h>
#include <string.h>
#include <csetjmp>

using namespace std;

const char * REGISTERS[] = {
	"REG_R8",  "REG_R9",  "REG_R10", "REG_R11", "REG_R12",
    "REG_R13", "REG_R14", "REG_R15", "REG_RDI", "REG_RSI",
	"REG_RBP", "REG_RBX", "REG_RDX", "REG_RAX", "REG_RCX",
};

const int REG_LEN = 15;

sigjmp_buf jbuf;

void checkError(int code, string message) {
	if (code < 0) {
		perror(message.c_str());
		exit(EXIT_FAILURE);
	}	
}

void new_handler228(int sig, siginfo_t *info, void *context) {
    if ((*info).si_signo == SIGSEGV) {
    	siglongjmp(jbuf, 1);	
    }
}

void dump_address(void* addr) {
	long long start_addr = (long long)((char *)addr - 30);
	start_addr -= start_addr % 16;
	long long end_addr = start_addr + 16 * 8;
	printf("Memory dump:");
	for (long long address = start_addr; address < end_addr; address++) {
    	struct sigaction act;
  		memset(&act, 0, sizeof(act));
   		act.sa_sigaction = new_handler228;
    	act.sa_flags = SA_SIGINFO | SA_NODEFER;
    	checkError(sigaction(SIGSEGV, &act, NULL), "sigaction"); 
   		char* zn = (char *)(address);
   		if (address % 16 == 0) {
   			printf("\n%p :\t", (void *)(address));
   		}
    	if (sigsetjmp(jbuf, 0) == 0) {
       		printf("   %02x", (int)((unsigned char)(zn[0])));
   		} else {
        	printf("   --\n");
    	}

	}
	printf("\n\n");
}

void dump_registers(ucontext_t *context) {
	printf("Register dump\n");
	for (int i = 0; i < REG_LEN; i++) {
        printf("%-15s = 0x%x\n", REGISTERS[i], (unsigned int)(*context).uc_mcontext.gregs[i]);
    }
}

void handler(int sig, siginfo_t *info, void *context) {
	if ((*info).si_signo == SIGSEGV) {
		printf("Segmentation fault cased in %p\n", (*info).si_addr);
		switch ((*info).si_code) {
			case SEGV_ACCERR:
				printf("Permission error\n");
				break;
			case SEGV_MAPERR:
				printf("Accessing an address, which is not mapped\n");
				break;
			default:
				printf("Unknown error\n");
        }
        dump_address((*info).si_addr);
        dump_registers((ucontext_t *) context);		
	}
	exit(EXIT_FAILURE);
}
       
int main(int argc, char* argv[]) {
	struct sigaction act;
	memset(&act, 0, sizeof(act));
    act.sa_sigaction = handler;
    act.sa_flags = SA_SIGINFO | SA_NODEFER;
    checkError(sigaction(SIGSEGV, &act, NULL), "sigaction");
    
    char *c = (char *)("kek");
	printf("String starts from:%p\n", (void *)(c));
	c[4] = 'x';
}