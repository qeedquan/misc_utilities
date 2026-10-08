#!/usr/bin/env python3

"""

https://aleksandarhaber.com/how-to-detect-an-object-in-a-scene-by-using-feature-matching-opencv-tutorial/

The way these detectors work is that you give it a scene image and a query image
The feature detector will find keypoints/features in the scene and the query image
Then a matcher is used to create a correspondence between those keypoints/features to localize where it is in the scene

"""

import sys
import cv2

def main():
    if len(sys.argv) != 3:
        print("usage: <scene_image> <query_image>")
        sys.exit(2)

    scene_image = cv2.imread(sys.argv[1],cv2.IMREAD_GRAYSCALE)
    query_image = cv2.imread(sys.argv[2],cv2.IMREAD_GRAYSCALE)

    # Use ORB detector to find keypoints and descriptors
    orb_detector = cv2.ORB_create()
    keypoint_query, descriptors_query = orb_detector.detectAndCompute(query_image, None)
    keypoint_scene, descriptors_scene = orb_detector.detectAndCompute(scene_image, None)

    # Use brute forcer matcher to match features
    bf = cv2.BFMatcher(cv2.NORM_HAMMING, crossCheck=True)
    detected_matches = bf.match(descriptors_query, descriptors_scene)
    sorted_detected_matches = sorted(detected_matches, key = lambda x: x.distance)

    # Use the top matches, the lower matches are superfluous and can point to other images in the scene
    top_matches = min(len(sorted_detected_matches), 10)
    combined_image = cv2.drawMatches(query_image, keypoint_query,
                                     scene_image, keypoint_scene,
                                     sorted_detected_matches[:top_matches], None, flags=2)

    print("Keypoints: ")
    for match in sorted_detected_matches:
        index_q = match.queryIdx
        index_s = match.trainIdx
        print("query: %s scene:%s" % (keypoint_query[index_q].pt, keypoint_scene[index_s].pt))

    cv2.imwrite("detected_image.png", combined_image)

main()
