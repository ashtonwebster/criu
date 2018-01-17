#include  <stdio.h>
#include  <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

void  main(void)
{
    pid_t  pid;
    pid = fork();
    int fd;
    if (pid == 0) {
        // child process
        fd = open("a.txt", O_RDONLY);
        while (1) { sleep(1); }
    }
    else {
        while (1) { sleep(1); }
    }
}
