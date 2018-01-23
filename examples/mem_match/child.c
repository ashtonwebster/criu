#include <unistd.h>
#include <stdio.h>
#include <string.h>

int main() {
    struct mystruct {
		char magic[8];
		char data[8];
		unsigned char mylong[10];
    };
    struct mystruct s;
    memcpy(&s.magic, "awmagic", strlen("banana") + 1);
    memcpy(&s.data, "123456", strlen("123456") + 1);
        s.mylong[0] = 0x12;
        s.mylong[1] = 0x34;
        s.mylong[2] = 0x56;
        s.mylong[3] = 0x78;
        s.mylong[4] = 0x90;
    while (1) {
		sleep(1);
    }
    return 0;
}
