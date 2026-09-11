input = "/ata0"
output = "/host.host/disk.img"
memsize = 104857600

xbdPartitionFun input, 2

fd = open(input, 0)
mem = calloc(memsize, 1)
read fd, mem, memsize

fd2 = creat(output, 511)
write fd2, mem, memsize

close fd
close fd2

xbdPartitionFun input, 3

