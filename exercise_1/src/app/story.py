import os

from life import *
from PIL import Image
from scipy import ndimage


def save_story(file_list, output_file):
    history = []
    frame_size = (500, 500)
    for file in file_list:
        image = load_pgm_snapshot(file)
        resized_image = ndimage.zoom(image, (frame_size[0] / image.shape[0], frame_size[1] / image.shape[1]))
        pil_image = Image.fromarray(resized_image)
        history.append(pil_image)

    history[0].save(output_file, save_all=True, append_images=history[1:], duration=600, loop=0)


if __name__ == '__main__':
    path = '../../snapshot'
    file_list = list(map(lambda file: os.path.join(path, file), sorted(os.listdir(path))))
    output_file = '../../simulation/simulation.gif'
    save_story(file_list, output_file)
