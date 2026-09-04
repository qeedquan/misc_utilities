#!/usr/bin/env python3

"""

Focuses on second-order derivatives and looks for strong curvature changes.
Blobs are identified via the DoH approach by using the determinant of the Hessian matrix.
The local curvature of an image is represented by the Hessian matrix.
The local maxima in the Hessian’s determinant can be used to locate blobs of various sizes and forms.

Det(H) = Ixx*Iyy - (Ixy)^2

Where Ixx, Iyy, Ixy are second-order partial derivatives

"""


from skimage import feature, color, io
import matplotlib.pyplot as plt

image = color.rgb2gray(io.imread("example-image.jpg"))
blobs_doh = feature.blob_doh(image, max_sigma=30, threshold=0.01)

# Display
fig, ax = plt.subplots()
ax.imshow(image, cmap='gray')
for y, x, r in blobs_doh:
    c = plt.Circle((x, y), r, color='blue', linewidth=2, fill=False)
    ax.add_patch(c)
plt.show()
