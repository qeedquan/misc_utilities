#!/usr/bin/env python3

"""

For every pixel in the image a program calculated a histogram (first plane of color space) of the neighbouring of size 2*size+1. and assigned the value of the most frequently occurring value.
The result looks almost like an oil painting.
The dynamic ratio decreases the image dynamic range and that increases the oil painting effect.

"""

import argparse
import cv2

def main():
    parser = argparse.ArgumentParser(prog="oilpaint", description="Make an oil painting of the image")
    parser.add_argument('argv', metavar='', type=str, nargs='+', help="")
    parser.add_argument("--neighbor-size", type=int, default=10, help="specify neighbor size")
    parser.add_argument("--dynratio", type=int, default=1, help="specify ratio to divide before histogram processing")
    args = parser.parse_args()

    if len(args.argv) != 2:
        print("usage: [options] input output")
    
    img = cv2.imread(args.argv[0]);
    oil = cv2.xphoto.oilPainting(img, args.neighbor_size, args.dynratio, cv2.COLOR_BGR2Lab);
    cv2.imwrite(args.argv[1], oil)

main()
