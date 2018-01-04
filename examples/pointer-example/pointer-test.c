#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main() {
    struct mystruct {
		char magic[8];
		char* data;
    };
    struct mystruct *s, *s2;
    s = malloc(sizeof(struct mystruct));
    memcpy(s->magic, "awmagic", strlen("awmagic") + 1);
    s->data = malloc(10);
    memcpy(s->data, "123456", strlen("123456") + 1);
    s2 = malloc(sizeof(struct mystruct));
    memcpy(s2->magic, "awmagic", strlen("awmagic") + 1);
    s2->data = malloc(10);
    memcpy(s2->data, "898989", strlen("898989") + 1);
    while (1) {
	printf("%s %s\n", s->data, s2->data);
    	sleep(1);
    }
    return 0;
}
