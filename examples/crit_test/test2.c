#include <unistd.h>
#include <stdio.h>
#include <string.h>

int main() {
    struct mystruct {
		char magic[8];
		char data[8];
    };
    struct mystruct s;
    memcpy(&s.magic, "banana", strlen("banana") + 1);
    memcpy(&s.data, "123456", strlen("123456") + 1);
    while (1) {
	    printf("%s\n", s.data);
    sleep(1);
    }
    return 0;
}
