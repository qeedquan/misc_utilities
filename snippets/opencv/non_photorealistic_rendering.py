#!/usr/bin/env python3

"""

Various filters to make the picture look more like an artist drew it

"""

import sys
import cv2

def main():
    if len(sys.argv) != 2:
        print("usage: file")
        sys.exit(2)

    img = cv2.imread(sys.argv[1], cv2.IMREAD_COLOR)
    if img is None:
        print("Failed to load image")
        sys.exit(1)

    out = cv2.edgePreservingFilter(img, 1)
    cv2.imwrite("edge_preserving_1.png", out)

    out = cv2.edgePreservingFilter(img, 2)
    cv2.imwrite("edge_preserving_2.png", out)

    out = cv2.detailEnhance(img)
    cv2.imwrite("detail_enhanced.png", out)

    out, xout = cv2.pencilSketch(img)
    cv2.imwrite("pencil_sketch.png", out)
    cv2.imwrite("pencil_sketch_color.png", xout)

    out = cv2.stylization(img)
    cv2.imwrite("stylization.png", out)

main()
