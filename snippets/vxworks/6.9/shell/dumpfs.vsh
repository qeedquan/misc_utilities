input = "/ram"
output = "host:/disk.img"
memsize = 0x2000000

# simulate xbdPartitionFun
cfd = open(input, 0)
ioctl cfd, 0xbd000003, 2
close cfd
sleep 1

fd = open(input, 0)

mem = calloc(memsize, 1)
read fd, mem, memsize

fd2 = creat(output, 511)
write fd2, mem, memsize

close fd
close fd2

# simulate xbdPartitionFun
cfd = open(input, 0)
ioctl cfd, 0xbd000004, 2
close cfd
sleep 1

