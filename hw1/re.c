#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

int main(int argc, char * argv[])
{ 
	printf("%s\n", argv[1]);
	printf("%d\n", atoi(argv[1])); 


    return atoi(argv[1]); // Never get there normally
}