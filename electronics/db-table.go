package main

import (
	"fmt"
	"math"
	"os"
	"strconv"
	"text/tabwriter"
)

func main() {
	tw := tabwriter.NewWriter(os.Stdout, 1, 4, 1, ' ', 0)
	defer tw.Flush()

	fmt.Fprintf(tw, "db\tamplitude\tpower\n")
	for db := -100.0; db <= 100.0; db++ {
		fmt.Fprintf(tw, "%.0f\t%v\t%v\n", db, fflt(db2amp(db)), fflt(db2pow(db)))
	}
}

func amp2db(a float64) float64 {
	return 20 * math.Log10(a)
}

func pow2db(p float64) float64 {
	return 10 * math.Log10(p)
}

func db2amp(db float64) float64 {
	return math.Pow(10, db/20)
}

func db2pow(db float64) float64 {
	return math.Pow(10, db/10)
}

func fflt(x float64) string {
	return strconv.FormatFloat(x, 'f', -1, 64)
}
