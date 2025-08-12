#!/usr/bin/env python3

import argparse
import sys
import cv2 as cv
import numpy as np

def tune(name, args):
    src = cv.imread(cv.samples.findFile(name), cv.IMREAD_COLOR)
    gray = cv.cvtColor(src, cv.COLOR_BGR2GRAY)
    gray = cv.medianBlur(gray, 5)

    try:
        for min_distance in range(1, args.min_distance + 1):
            for param1 in range(1, args.param1 + 1):
                for param2 in range(1, args.param2 + 1):
                    for min_radius in range(1, args.min_radius + 1):
                        for max_radius in range(1, args.max_radius + 1):
                            circles = cv.HoughCircles(gray, cv.HOUGH_GRADIENT, 1, min_distance,
                                                        param1=param1, param2=param2,
                                                        minRadius=min_radius, maxRadius=max_radius)
                            print(len(circles), min_distance, param1, param2, min_radius, max_radius)
    except:
        pass

def detect(name, args):
    src = cv.imread(cv.samples.findFile(name), cv.IMREAD_COLOR)
    gray = cv.cvtColor(src, cv.COLOR_BGR2GRAY)
    gray = cv.medianBlur(gray, 5)

    circles = cv.HoughCircles(gray, cv.HOUGH_GRADIENT, 1, args.min_distance,
                               param1=args.param1, param2=args.param2,
                               minRadius=args.min_radius, maxRadius=args.max_radius)

    if circles is not None:
        circles = np.uint16(np.around(circles))
        for i in circles[0, :]:
            center = (i[0], i[1])
            # circle center
            cv.circle(src, center, 1, (0, 100, 100), 3)
            # circle outline
            radius = i[2]
            cv.circle(src, center, radius, (255, 0, 255), 3)
    
    cv.imshow("detected circles", src)
    while True:
        key = cv.waitKey(0)
        if key == 27:
            break

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('argv', metavar='', type=str, nargs='+', help="")
    parser.add_argument("--min_distance", help="specify min distance", default=8, type=int)
    parser.add_argument("--param1", help="specify param1", default=100, type=int)
    parser.add_argument("--param2", help="specify param2", default=30, type=int)
    parser.add_argument("--min_radius", help="specify min radius", default=1, type=int)
    parser.add_argument("--max_radius", help="specify max radius", default=30, type=int)
    parser.add_argument("--tune", help="automatic parameters tuning", default=False, type=bool)
    args = parser.parse_args()

    if not args.tune:
        detect(args.argv[0], args)
    else:
        tune(args.argv[0], args)

main()
