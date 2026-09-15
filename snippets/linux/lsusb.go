// +build linux

// ported from suckless
package main

import (
	"bufio"
	"flag"
	"fmt"
	"os"
	"path/filepath"
	"sort"
)

type Device struct {
	Bus, Dev, Pid, Vid int
}

type DeviceSlice []Device

func (p DeviceSlice) Len() int      { return len(p) }
func (p DeviceSlice) Swap(i, j int) { p[i], p[j] = p[j], p[i] }
func (p DeviceSlice) Less(i, j int) bool {
	switch {
	case p[i].Bus != p[j].Bus:
		return p[i].Bus > p[j].Bus
	case p[i].Dev != p[j].Dev:
		return p[i].Dev > p[j].Dev
	case p[i].Pid != p[j].Pid:
		return p[i].Pid > p[j].Pid
	default:
		return p[i].Vid > p[j].Vid
	}
}

var devices []Device

func main() {
	flag.Parse()
	filepath.Walk("/sys/bus/usb/devices", walk)
	sort.Sort(DeviceSlice(devices))
	for _, d := range devices {
		fmt.Printf("Bus %03d Device %03d: ID %04x:%04x\n",
			d.Bus, d.Dev, d.Pid, d.Vid)
	}
}

func walk(path string, info os.FileInfo, err error) error {
	if err != nil {
		return err
	}

	return lsusb(filepath.Join(path, "uevent"))
}

func lsusb(path string) error {
	f, err := os.Open(path)
	if err != nil {
		return nil
	}
	defer f.Close()

	var bus, dev, pid, vid, i int
	s := bufio.NewScanner(f)
	for s.Scan() {
		line := s.Text()
		n1, _ := fmt.Sscanf(line, "BUSNUM=%d\n", &bus)
		n2, _ := fmt.Sscanf(line, "DEVNUM=%d\n", &dev)
		n3, _ := fmt.Sscanf(line, "PRODUCT=%x/%x\n", &pid, &vid)
		if n1 != 0 || n2 != 0 || n3 != 0 {
			i++
		}
		if i == 3 {
			devices = append(devices, Device{bus, dev, pid, vid})
			break
		}
	}

	return nil
}
