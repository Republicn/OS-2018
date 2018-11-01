#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

void func(int *x) {
    assert(x); - проверяет, что X не NULL
	*x = 100500;
}

int main() {
	int x = 42;
	static int y;
	int *z = malloc(sizeof(z));
	func(&x);
	func(NULL); - segmentetion error
}