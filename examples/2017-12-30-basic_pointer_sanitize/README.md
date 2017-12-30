This example searches for the string "awmagic" in memory and uses it to find
the pointer to the printed data (123456).  The pointer is then

```
git checkout pointer-test
sudo make install
# start 2 screens via "screen" command and Ctrl+A C

# screen 1
make pointer-test
./pointer-test
# printing 123456

# screen 2
sudo su
ps -C pointer-test
pid=<insert pid>
criu dump -vvvv -o dump.log -t $pid --images-dir imgs4 --shell-job
# restoring
criu restore --images-dir imgs/ --shell-job
# now resumes printing 654321
```
