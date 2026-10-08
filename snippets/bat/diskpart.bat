REM Format partition as FAT32

diskpart
list disk
select disk 1
clean
create active partition
active
select partition 1
format fs=fat32

