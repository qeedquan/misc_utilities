echo "version" > sysinfo.txt
echo ""
version >> sysinfo.txt

echo "adrSpaceShow" >> sysinfo.txt
echo ""
C adrSpaceShow > adr_space_show.txt
cat adr_space_show.txt >> sysinfo.txt

echo "ifconfig" >> sysinfo.txt
echo ""
ifconfig -a >> sysinfo.txt

echo "vxbDrvShow" >> sysinfo.txt
echo ""
C vxbDrvShow > vxb_drv_show.txt
cat vxb_drv_show.txt >> sysinfo.txt

echo "iosDrvShow" >> sysinfo.txt
echo ""
C iosDrvShow > ios_drv_show.txt
cat ios_drv_show.txt >> sysinfo.txt

echo "usbShow" >> sysinfo.txt
echo ""
C usbShow > usb_show.txt
cat usb_show.txt >> sysinfo.txt

echo "vxbDevShow" >> sysinfo.txt
echo ""
C vxbDevShow > vxb_dev_show.txt
cat vxb_dev_show.txt >> sysinfo.txt

echo "vmContextShow" >> sysinfo.txt
echo ""
C vmContextShow > vmContextShow.txt
cat vmContextShow.txt >> sysinfo.txt

