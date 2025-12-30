// implemented algorithm from
// https://gamedevelopment.tutsplus.com/tutorials/how-to-use-bsp-trees-to-generate-game-maps--gamedev-12268
package main

import (
	"flag"
	"fmt"
	"image"
	"image/color"
	"image/draw"
	"image/png"
	"log"
	"math/rand"
	"os"
	"time"
)

func randNum(min, max int) int {
	return min + rand.Intn(max-min+1)
}

func abs(x int) int {
	if x < 0 {
		return -x
	}
	return x
}

func makeRect(x, y, w, h int) image.Rectangle {
	return image.Rect(x, y, x+w, y+h)
}

type Leaf struct {
	Bounds      image.Rectangle
	Left        *Leaf
	Right       *Leaf
	Room        image.Rectangle
	Halls       []image.Rectangle
	MinLeafSize int
	MaxLeafSize int
	TileSize    int
}

func NewLeaf(x, y, width, height int) *Leaf {
	tileSize := *mapTileSize
	return &Leaf{
		Bounds:      makeRect(x, y, width, height),
		TileSize:    tileSize,
		MinLeafSize: 6 * tileSize,
		MaxLeafSize: 20 * tileSize,
	}
}

func NewTree(width, height int) *Leaf {
	// create root of all leaves
	root := NewLeaf(0, 0, width, height)
	leafs := []*Leaf{root}

	split := true
	for split {
		split = false
		for _, l := range leafs {
			// if this leaf is not already split
			if l.Left == nil && l.Right == nil {
				// if this leaf is too big or over 75% chance
				width := l.Bounds.Dx()
				height := l.Bounds.Dy()
				if width > l.MaxLeafSize || height > l.MaxLeafSize || rand.Float64() > 0.25 {
					if l.Split() {
						// if we did split, push child leaves to vector so we can loop them next
						leafs = append(leafs, l.Left)
						leafs = append(leafs, l.Right)
						split = true
					}
				}
			}
		}
	}
	root.CreateRooms()

	return root
}

func (l *Leaf) Split() bool {
	// begin splitting the leaf into two children
	if l.Left != nil || l.Right != nil {
		// already split, abort
		return false
	}

	// determine direction of split
	// if width is >25% larger than height, split vertically
	// if height is >25% larger than width, split horizontally
	// otherwise split randomly
	width := l.Bounds.Dx()
	height := l.Bounds.Dy()
	splitH := rand.Float64() > 0.5
	if width > height && float64(width)/float64(height) >= 1.25 {
		splitH = false
	} else if height > width && float64(height)/float64(width) >= 1.25 {
		splitH = true
	}

	// determine the max height or width
	max := width - l.MinLeafSize
	if splitH {
		max = height - l.MinLeafSize
	}
	if max <= l.MinLeafSize {
		// area is too small to split, so just return
		return false
	}

	// determine where to split
	split := randNum(l.MinLeafSize, max)

	// create left and right children based on direction of split
	x := l.Bounds.Min.X
	y := l.Bounds.Min.Y
	if splitH {
		l.Left = NewLeaf(x, y, width, split)
		l.Right = NewLeaf(x, y+split, width, height-split)
	} else {
		l.Left = NewLeaf(x, y, split, height)
		l.Right = NewLeaf(x+split, y, width-split, height)
	}

	return true
}

func (l *Leaf) GetRoom() image.Rectangle {
	if l.Room != image.ZR {
		return l.Room
	}

	var lRoom, rRoom image.Rectangle
	if l.Left != nil {
		lRoom = l.Left.GetRoom()
	}
	if l.Right != nil {
		rRoom = l.Right.GetRoom()
	}

	if lRoom == image.ZR && rRoom == image.ZR {
		return image.ZR
	} else if rRoom == image.ZR {
		return lRoom
	} else if lRoom == image.ZR {
		return rRoom
	} else if rand.Float64() > 0.5 {
		return lRoom
	} else {
		return rRoom
	}
}

func (l *Leaf) CreateHall(left, right image.Rectangle) {
	l.Halls = l.Halls[:0]
	p1 := image.Pt(
		randNum(left.Min.X+1, left.Max.X-2),
		randNum(left.Min.Y+1, left.Max.Y-2),
	)
	p2 := image.Pt(
		randNum(right.Min.X+1, right.Max.X-2),
		randNum(right.Min.Y+1, right.Max.Y-2),
	)
	w := p2.X - p1.X
	h := p2.Y - p1.Y

	if w < 0 {
		if h < 0 {
			if rand.Float64() < 0.5 {
				l.Halls = append(l.Halls, makeRect(p2.X, p1.Y, abs(w), 1))
				l.Halls = append(l.Halls, makeRect(p2.X, p2.Y, 1, abs(h)))
			} else {
				l.Halls = append(l.Halls, makeRect(p2.X, p2.Y, abs(w), 1))
				l.Halls = append(l.Halls, makeRect(p1.X, p2.Y, 1, abs(h)))
			}
		} else if h > 0 {
			if rand.Float64() < 0.5 {
				l.Halls = append(l.Halls, makeRect(p2.X, p1.Y, abs(w), 1))
				l.Halls = append(l.Halls, makeRect(p2.X, p1.Y, 1, abs(h)))
			} else {
				l.Halls = append(l.Halls, makeRect(p2.X, p2.Y, abs(w), 1))
				l.Halls = append(l.Halls, makeRect(p1.X, p1.Y, 1, abs(h)))
			}
		} else {
			l.Halls = append(l.Halls, makeRect(p2.X, p2.Y, abs(w), 1))
		}
	} else if w > 0 {
		if h < 0 {
			if rand.Float64() < 0.5 {
				l.Halls = append(l.Halls, makeRect(p1.X, p2.Y, abs(w), 1))
				l.Halls = append(l.Halls, makeRect(p1.X, p2.Y, 1, abs(h)))
			} else {
				l.Halls = append(l.Halls, makeRect(p1.X, p1.Y, abs(w), 1))
				l.Halls = append(l.Halls, makeRect(p2.X, p2.Y, 1, abs(h)))
			}
		} else if h > 0 {
			if rand.Float64() < 0.5 {
				l.Halls = append(l.Halls, makeRect(p1.X, p1.Y, abs(w), 1))
				l.Halls = append(l.Halls, makeRect(p2.X, p1.Y, 1, abs(h)))
			} else {
				l.Halls = append(l.Halls, makeRect(p1.X, p2.Y, abs(w), 1))
				l.Halls = append(l.Halls, makeRect(p1.X, p1.Y, 1, abs(h)))
			}
		} else {
			l.Halls = append(l.Halls, makeRect(p1.X, p1.Y, abs(w), 1))
		}
	} else {
		if h < 0 {
			l.Halls = append(l.Halls, makeRect(p2.X, p2.Y, 1, abs(h)))
		} else {
			l.Halls = append(l.Halls, makeRect(p1.X, p1.Y, 1, abs(h)))
		}
	}

	for i, r := range l.Halls {
		l.Halls[i] = makeRect(r.Min.X, r.Min.Y, r.Dx()*l.TileSize/2, r.Dy()*l.TileSize/2)
	}
}

// generates all the rooms and hallways for this leaf and all of its children
func (l *Leaf) CreateRooms() {
	if l.Left != nil || l.Right != nil {
		// if split, go into children leafs
		if l.Left != nil {
			l.Left.CreateRooms()
		}
		if l.Right != nil {
			l.Right.CreateRooms()
		}
		if l.Left != nil && l.Right != nil {
			l.CreateHall(l.Left.GetRoom(), l.Right.GetRoom())
		}
	} else {
		// ready to make a room
		x := l.Bounds.Min.X
		y := l.Bounds.Min.Y
		width := l.Bounds.Dx()
		height := l.Bounds.Dy()
		roomSize := image.Pt(randNum(3, width-2), randNum(3, height-2))

		// place the room within the Leaf, but don't put it right
		// against the side of the Leaf (that would merge rooms together)
		roomPos := image.Pt(randNum(1, width-roomSize.X-1), randNum(1, height-roomSize.Y-1))

		x0 := x + roomPos.X
		y0 := y + roomPos.Y
		x1 := x0 + roomSize.X
		y1 := y0 + roomSize.Y
		l.Room = image.Rect(x0, y0, x1, y1)
	}
}

func WalkTree(l *Leaf, f func(l *Leaf)) {
	if l == nil {
		return
	}
	f(l)
	WalkTree(l.Left, f)
	WalkTree(l.Right, f)
}

var (
	mapWidth    = flag.Int("w", 512, "map width")
	mapHeight   = flag.Int("h", 512, "map height")
	mapTileSize = flag.Int("t", 16, "tile size")
)

func main() {
	log.SetFlags(0)
	log.SetPrefix("bsp_gen_map: ")
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() < 1 {
		usage()
	}

	rand.Seed(time.Now().UnixNano())
	img := image.NewRGBA(image.Rect(0, 0, *mapWidth, *mapHeight))
	root := NewTree(*mapWidth, *mapHeight)
	WalkTree(root, func(l *Leaf) {
		if l.Room != image.ZR {
			draw.Draw(img, l.Room, image.NewUniform(color.White), image.ZP, draw.Src)
		}
		for _, r := range l.Halls {
			draw.Draw(img, r, image.NewUniform(color.White), image.ZP, draw.Src)
		}
	})

	f, err := os.Create(flag.Arg(0))
	ck(err)
	ck(png.Encode(f, img))
	ck(f.Close())
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: bsp_gen_map <file>")
	flag.PrintDefaults()
	os.Exit(2)
}

func ck(err error) {
	if err != nil {
		log.Fatal(err)
	}
}
