
Example of tcp connection removal.  In policy/json/tcp.json, we specify an
destination IP "127.0.0.1" which is removed on restore.  This is accomplished
by restoring the connection to a "TCP\_CLOSE" state


```
# start 3 screens via "screen" command and Ctrl+a c
# switch screens with Ctrl+a n

# screen 1: echo server
make echo
./echo
# printing 55555

# screen 2: echo client
telnet localhost 55555
```

You could also start other connections from
other (non-localhost) IPs.  These would not be removed on the restore
and should continue to operate after the restore.

```
# screen 3: dump/restore
sudo su
ps -C 
pid=<insert pid>
mkdir -p imgs
criu dump -vvvv -o dump.log -t $pid --images-dir imgs --tcp-established --shell-job --policy-path /home/terrapin/criu/policy/imgs/tcp.img
#verify it worked by searching for "ip address" in the output log
vim imgs/dump.log
# restoring
criu restore --images-dir imgs/ --shell-job -v4 -o rst.log --tcp-established
# Should see "Done" indicating a connection was terminated.
```
