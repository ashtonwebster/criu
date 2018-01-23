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
	execl("/home/ashton/criu/examples/mem_match/child", 
			"/home/ashton/criu/examples/mem_match/child", 
			(char *)NULL);
    }
    else {
        while (1) { sleep(1); }
    }
}
