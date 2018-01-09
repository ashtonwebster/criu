This example searches for the string "awmagic" in memory and uses it to find
a double pointer in memory (a pointer to a pointer).  It replaces the data
pointed to by the pointer with "654321".

```
git checkout pointer-test
sudo make install
# start 2 screens via "screen" command and Ctrl+a c
# switch screens with Ctrl+a n

# screen 1
make double-pointer
./double-pointer
# printing 123456

# screen 2
sudo su
ps -C pointer-test
pid=<insert pid>
mkdir -p imgs
criu dump -vvvv -o dump.log -t $pid --images-dir imgs --shell-job --policy-path /home/terrapin/criu/policy/imgs/example.img
#verify it worked by searching for "awmagic" in the output log
vim imgs/dump.log
# restoring
criu restore --images-dir imgs/ --shell-job
# now resumes printing 654321
```
