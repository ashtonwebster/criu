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
	execl("/bin/sleep", "/bin/sleep", "99999999", (char *)NULL);
    }
    else {
        while (1) { sleep(1); }
    }
}
