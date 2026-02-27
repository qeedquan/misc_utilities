#!/bin/sh

echo "Version"
vcgencmd version
echo ""

echo "Config"
vcgencmd get_config int
vcgencmd get_config str
echo ""

echo "Frequencies"
for id in arm core h264 isp v3d uart pwm emmc pixel vec hdmi dpi
do
	echo -n "$id: "
	vcgencmd measure_clock $id
done
vcgencmd hdmi_ntsc_freqs
vcgencmd get_throttled
echo ""

echo "Memory"
vcgencmd get_mem arm
vcgencmd get_mem gpu
vcgencmd mem_reloc_stats
vcgencmd get_hvs_asserts
vcgencmd get_rsts
echo ""

echo "Display"
vcgencmd get_lcd_info
vcgencmd get_camera
echo ""

echo "Power Management"
vcgencmd pm_get_status
echo ""

echo "Voltage Sensors"
for id in core sdram_c sdram_i sdram_p
do
	echo -n "$id: "
	vcgencmd measure_volts $id
done
echo ""

echo "Temperature Sensors"
vcgencmd measure_temp
echo ""

echo "Codec Enabled"
for id in H265 H264 MPG2 WVC1 MPG4 MJPG WMV9
do
	echo -n "$id: "
	vcgencmd codec_enabled $id
done
echo ""

# /proc/cpuinfo reads from these locations for info
# 28: serial
# 30: revision
echo "OTP"
vcgencmd otp_dump
echo ""

# https://github.com/raspberrypi/firmware/blob/master/boot/overlays/README
# add 'dtdebug=on' to /boot/config.txt
# to get more messages for device tree initialization from firmware
echo "Debug Messages"
sudo vcdbg log msg
echo ""

echo "GPIO Mappings"
raspi-gpio get 0-53
echo ""
