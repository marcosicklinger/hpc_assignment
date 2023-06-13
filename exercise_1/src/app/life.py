import imageio.v2 as imageio
import matplotlib.pyplot as plt
import matplotlib.colors as mcolors
import numpy as np


def load_pgm_snapshot(file):
    image = np.array(imageio.imread(file))
    return image.astype(np.uint8)


def binary_cmap():
    colors = ['black', 'white']
    cmap = mcolors.ListedColormap(colors)
    norm = mcolors.Normalize(vmin=0, vmax=255)
    return cmap, norm


def plot_image(image):
    cmap, norm = binary_cmap()
    plt.imshow(image, cmap=cmap, norm=norm)
    plt.axis('off')
    plt.show()


if __name__ == '__main__':
    file = '../../../snapshot/20.pgm'
    image = load_pgm_snapshot(file)
    plot_image(image)
