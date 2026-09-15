from keras.datasets import mnist
from PIL import Image
import os

folder = "mnist_data/"
try:
    os.mkdir(folder)
except:
    pass

(train_images, train_labels), (test_images, test_labels) = mnist.load_data()
hist = dict.fromkeys(test_labels, 0)

for i in range(len(test_images)):
    lb = test_labels[i]
    im = Image.fromarray(test_images[i])
    im.save("{}/{}-{}.png".format(folder, lb, hist[lb]))
    hist[lb] += 1

