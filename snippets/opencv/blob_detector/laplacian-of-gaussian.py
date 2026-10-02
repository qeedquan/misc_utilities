#!/usr/bin/env python3

"""

https://opencv.org/blog/blob-detection-using-opencv/

Classical blob detector

Image is smoothed with a gaussian kernel then filtered using laplacian operator (second derivative)

LoG(x, y, σ) = σ^2 * ∇^2(Convolve(G(x, y, σ), I(x, y)))

G(x,y,σ): Gaussian function with standard deviation σ
∇^2: Laplacian operator
I(x,y): Input image

Although the LoG approach can identify blobs of different sizes effectively, it may return multiple strong responses for a single blob.
Often post-processing techniques like non-maximum suppression are applied to keep only the strongest response for each blob and eliminate duplicates.

"""

from skimage import feature, color, io
import matplotlib.pyplot as plt
 
image = color.rgb2gray(io.imread("example-image.jpg"))
blobs_log = feature.blob_log(image, max_sigma=30, num_sigma=10, threshold=0.1)
 
# Compute radii
blobs_log[:, 2] = blobs_log[:, 2] * (2 ** 0.5)
 
# Display
fig, ax = plt.subplots()
ax.imshow(image, cmap='gray')
for y, x, r in blobs_log:
    c = plt.Circle((x, y), r, color='red', linewidth=2, fill=False)
    ax.add_patch(c)
plt.show()
