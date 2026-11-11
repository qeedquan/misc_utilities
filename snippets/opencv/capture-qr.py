#!/usr/bin/env python

# https://note.nkmk.me/en/python-opencv-qrcode/

import cv2
import time

def main():
    cap = cv2.VideoCapture(0)
    qcd = cv2.QRCodeDetector()
    if not cap.isOpened():
        raise IOError("Cannot open camera")

    title = "Camera"
    fps = 0
    start = time.monotonic()
    while True:
        fps += 1
        end = time.monotonic()
        if end-start >= 1:
            title = "Camera (FPS: {})".format(fps/(end-start))
            fps = 0
            start = end

        ret, frame = cap.read()
        frame = cv2.resize(frame, (1024, 768), interpolation=cv2.INTER_LANCZOS4)

        detected, decoded_info, points, _ = qcd.detectAndDecodeMulti(frame)
        if detected:
            for s, p in zip(decoded_info, points):
                if s:
                    print(s)
                    color = (0, 255, 0)
                else:
                    color = (0, 0, 255)
                frame = cv2.polylines(frame, [p.astype(int)], True, color, 8)
                frame = cv2.putText(frame, s, p[0].astype(int), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 0, 255), 2, cv2.LINE_AA)


        cv2.imshow("Camera", frame)
        cv2.setWindowTitle("Camera", title)

        key = cv2.waitKey(1)
        if key == 27:
            break

    cap.release()
    cv2.destroyAllWindows()

main()

