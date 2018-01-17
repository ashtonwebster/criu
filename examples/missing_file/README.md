# Missing file recovery example

Here we have an example of a process with a child, where the 
child has a reference to a file which no longer exists. To simulate
this, we remove the file a.txt (referenced from the child process)
between the dump and restore processes.  The parent continues
running despite this.

```
$ ./fork &
$ ps -ef | grep fork
# should be two process, one as the parent and one as the child
# set pid to the parent PID
$ pid=<insertpid>
$ criu dump -t $pid --images-dir imgs/ -v4 -o dump.log --shell-job

# confirming it worked
$ tail imgs/dump.log
# should say "Dump successful"

# child will have file removed before restore
$ mv a.txt a.txt.backup
$ criu restore --images-dir imgs/ -d -vvv -o restore.log --shell-job
$ ps -ef  | grep fork
# should now only see the parent process

```
