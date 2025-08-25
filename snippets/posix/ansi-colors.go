package main

import "fmt"

func main() {
	col256()
	col8()
}

func col8() {
	const text = "tExTyOsIs"

	var bg [9]string
	for i := 1; i < len(bg); i++ {
		bg[i] = fmt.Sprintf("%dm", 40+i-1)
	}

	var fg [18]string
	fg[0], fg[1] = "m", "1m"
	for i := 2; i < len(fg); i++ {
		fg[i] = fmt.Sprintf("%dm", 30+(i-2)/2%8)
		if i&1 != 0 {
			fg[i] = "1;" + fg[i]
		}
	}

	align := len(text) + 2
	fmt.Printf("\033[1m%-*s\033[0m", align, " ")
	for i := range bg {
		fmt.Printf("\033[1m%-*s\033[0m", align, bg[i])
	}
	fmt.Println()
	for i := range fg {
		fmt.Printf("\033[1m%-*s\033[0m", align, fg[i])

		esc := fmt.Sprintf("\033[%s", fg[i])
		for j := range bg {
			if bg[j] != "" {
				esc += fmt.Sprintf("\033[%s", bg[j])
			}
			fmt.Printf("%s%-*s\033[0m", esc, align, text)
		}
		fmt.Println()
	}
	fmt.Println()
}

func col256() {
	const N = 256
	const S = 16

	var fg, bg [N]string
	for i := 0; i < N; i++ {
		fg[i] = fmt.Sprintf("\033[38;5;%dm", i)
		bg[i] = fmt.Sprintf("\033[48;5;%dm", i)
	}

	for i := 0; i < N; i += S {
		for j := 0; j < N; j += S {
			for k := 0; k < S; k++ {
				for l := 0; l < S; l++ {
					fmt.Printf("(%3d, %3d) %s%s%s\033[0m  ", i+k, j+l, fg[i+k], bg[j+l], "aBc")
				}
				fmt.Println()
			}
		}
	}
	fmt.Println()
}
