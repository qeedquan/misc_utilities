// +build linux

// ported from toybox
package main

import (
	"bufio"
	"encoding/csv"
	"flag"
	"fmt"
	"os"
	"os/user"
	"path/filepath"
	"strconv"
	"strings"
	"syscall"
)

type Proc struct {
	Cmd  string
	Pid  int
	User string
}

type File struct {
	Proc
	Name    string
	Fd      string
	Type    string
	Device  string
	SizeOff string
	Node    string
	Rw      rune
	Locks   rune

	Dev uint64
	Ino uint64
}

var (
	infos   []*syscall.Stat_t
	files   []*File
	pids    pidList
	numeric bool
	terse   bool

	lastShownPid int
	shownHeader  bool
)

func main() {
	flag.BoolVar(&terse, "t", false, "terse (pid only) output")
	flag.BoolVar(&numeric, "l", false, "list uids numerically")
	flag.Var(&pids, "p", "for given comma-separated pids only (default all pids)")
	flag.Usage = usage
	flag.Parse()

	for _, name := range flag.Args() {
		info, err := stat(name)
		ck(err)
		infos = append(infos, info)
	}

	if len(pids) == 0 {
		ck(filepath.Walk("/proc", func(path string, info os.FileInfo, err error) error {
			dir := filepath.Dir(path)
			if dir == "/proc" {
				pid, err := strconv.Atoi(filepath.Base(path))
				if err == nil {
					listPid(pid)
				}
			}
			return nil
		}))
	} else {
		for _, pid := range pids {
			ek(listPid(pid))
		}
	}

	for _, file := range files {
		printInfo(file)
	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: lsof [-lt] [-p PID1,PID2...] [NAME]...")
	flag.PrintDefaults()
	os.Exit(2)
}

func ck(err error) {
	if err != nil {
		fmt.Fprintln(os.Stderr, "lsof:", err)
		os.Exit(1)
	}
}

func ek(err error) {
	if err != nil {
		fmt.Fprintln(os.Stderr, "lsof:", err)
	}
}

func stat(name string) (*syscall.Stat_t, error) {
	info, err := os.Stat(name)
	if err != nil {
		return nil, err
	}

	st, ok := info.Sys().(*syscall.Stat_t)
	if !ok {
		return nil, fmt.Errorf("invalid stat type")
	}

	return st, nil
}

func major(dev uint64) uint64 {
	return (dev & 0xfff00) >> 8
}

func minor(dev uint64) uint64 {
	return ((dev & 0xfff00000) >> 12) | (dev & 0xff)
}

func listPid(pid int) error {
	buf, err := os.ReadFile(fmt.Sprintf("/proc/%d/stat", pid))
	if err != nil {
		return err
	}
	str := string(buf)

	pi := Proc{Cmd: "?", Pid: pid}
	op := strings.IndexRune(str, '(')
	cp := strings.IndexRune(str, ')')
	if op >= 0 && cp >= 0 {
		pi.Cmd = str[op+1 : cp]
	}

	info, err := os.Stat(fmt.Sprintf("/proc/%d", pid))
	if err == nil {
		st, ok := info.Sys().(*syscall.Stat_t)
		if ok {
			user, err := user.LookupId(fmt.Sprint(st.Uid))
			if !numeric && err == nil {
				pi.User = user.Username
			} else {
				pi.User = fmt.Sprint(st.Uid)
			}
		}
	}

	visitSymlink(&pi, "cwd", "cwd")
	visitSymlink(&pi, "rtd", "root")
	visitSymlink(&pi, "txt", "exe")
	visitMaps(&pi)
	visitFds(&pi)
	return nil
}

func visitSymlink(pi *Proc, name, path string) {
	var fn string
	fi := newFile(pi, "")
	if name != "" {
		fi.Fd = name
		fn = fmt.Sprintf("/proc/%d/%s", pi.Pid, path)
	} else {
		fi.Fd = path
		fillFlags(fi)
		fn = fmt.Sprintf("/proc/%d/fd/%s", pi.Pid, path)
	}

	fillStat(fi, fn)
	if fi.Name == "" {
		var err error
		fi.Name, err = os.Readlink(fn)
		if err != nil {
			fi.Name = fmt.Sprintf("%s (readlink: %s)", fn, err)
		}
	}
}

func visitMaps(pi *Proc) {
	f, err := os.Open(fmt.Sprintf("/proc/%d/maps", pi.Pid))
	if err != nil {
		return
	}
	defer f.Close()

	s := bufio.NewScanner(f)
	for s.Scan() {
		var offset, inode, namePos int64
		var device string

		line := s.Text()
		n, _ := fmt.Sscanf(line, "%*x-%*x %*s %x %d %d", &offset, &device, &inode, &namePos)
		if n >= 3 {
			if inode == 0 || device == "00:00" {
				continue
			}
			if offset != 0 {
				continue
			}

			fi := newFile(pi, "mem")
			fi.Name = strings.TrimSpace(line[namePos:])
			fillStat(fi, fi.Name)
		}
	}
}

func visitFds(pi *Proc) {
	f, err := os.Open(fmt.Sprintf("/proc/%d/fd", pi.Pid))
	if err != nil {
		fi := newFile(pi, "NOFD")
		fi.Name = fmt.Sprintf("%s", err)
		return
	}
	defer f.Close()

	for {
		dis, err := f.Readdir(1)
		if err != nil {
			break
		}

		if strings.HasPrefix(dis[0].Name(), ".") {
			continue
		}
		visitSymlink(pi, "", dis[0].Name())
	}
}

func newFile(pi *Proc, fd string) *File {
	fi := &File{
		Proc:  *pi,
		Fd:    fd,
		Type:  "unknown",
		Rw:    ' ',
		Locks: ' ',
	}
	files = append(files, fi)
	return fi
}

func fillFlags(fi *File) {
	f, err := os.Open(fmt.Sprintf("/proc/%d/fdinfo/%s", fi.Pid, fi.Fd))
	if err != nil {
		return
	}
	defer f.Close()

	var pos, flags int64
	n, _ := fmt.Fscanf(f, "pos: %d flags: %o", &pos, &flags)
	if n == 2 {
		flags &= syscall.O_ACCMODE
		if flags&int64(os.O_RDONLY) != 0 {
			fi.Rw = 'r'
		} else if flags&syscall.O_WRONLY != 0 {
			fi.Rw = 'w'
		} else {
			fi.Rw = 'u'
		}

		fi.SizeOff = fmt.Sprintf("0t%d", pos)
	}
}

func fillStat(fi *File, path string) {
	st, err := stat(path)
	if err != nil {
		return
	}

	switch st.Mode & syscall.S_IFMT {
	case syscall.S_IFBLK:
		fi.Type = "BLK"
	case syscall.S_IFCHR:
		fi.Type = "CHR"
	case syscall.S_IFDIR:
		fi.Type = "DIR"
	case syscall.S_IFIFO:
		fi.Type = "FIFO"
	case syscall.S_IFLNK:
		fi.Type = "LINK"
	case syscall.S_IFREG:
		fi.Type = "REG"
	case syscall.S_IFSOCK:
		fi.Type = "SOCK"
	default:
		fi.Type = fmt.Sprintf("0%03o", st.Mode&syscall.S_IFMT)
	}

	dev := st.Dev
	if mode := st.Mode & syscall.S_IFMT; mode == syscall.S_IFBLK || mode == syscall.S_IFCHR {
		dev = st.Rdev
	}
	if st.Mode&syscall.S_IFMT != syscall.S_IFSOCK {
		fi.Device = fmt.Sprintf("%d,%d", major(dev), minor(dev))
	}

	if mode := st.Mode & syscall.S_IFMT; mode == syscall.S_IFREG || mode == syscall.S_IFDIR {
		fi.SizeOff = fmt.Sprint(st.Size)
	}

	fi.Node = fmt.Sprintf("%d", st.Ino)

	fi.Dev = st.Dev
	fi.Ino = st.Ino
}

func printInfo(fi *File) {
	if len(infos) > 0 {
		i := 0
		for ; i < len(infos); i++ {
			if infos[i].Dev == fi.Dev && infos[i].Ino == fi.Ino {
				break
			}
		}
		if i == len(infos) {
			return
		}
	}

	if terse {
		if fi.Pid != lastShownPid {
			fmt.Println(fi.Pid)
			lastShownPid = fi.Pid
		}
	} else {
		if !shownHeader {
			fmt.Printf("%-15s %5s %10.10s %4s   %7s %18s %9s %10s %s\n", "COMMAND", "PID",
				"USER", "FD", "TYPE", "DEVICE", "SIZE/OFF", "NODE", "NAME")
			shownHeader = true
		}
		fmt.Printf("%-15s %5d %10.10s %4s%c%c %7s %18s %9s %10s %s\n",
			fi.Cmd, fi.Pid, fi.User,
			fi.Fd, fi.Rw, fi.Locks, fi.Type, fi.Device, fi.SizeOff,
			fi.Node, fi.Name)
	}
}

type pidList []int

func (pidList) String() string { return "" }
func (p *pidList) Set(s string) error {
	r := csv.NewReader(strings.NewReader(s))
	l, err := r.Read()
	if err != nil {
		return err
	}

	for i := range l {
		n, err := strconv.Atoi(l[i])
		if err != nil {
			return err
		}
		*p = append(*p, n)
	}

	return nil
}
