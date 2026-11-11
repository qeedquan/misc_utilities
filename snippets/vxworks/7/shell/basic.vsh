# Run a script in vxworks shell
# <basic.vsh to run it
filename = "myfile.txt"
fd = open(filename, 0x202, 0777)
ls
close(fd)
unlink(filename)
ls
