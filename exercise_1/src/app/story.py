import os

from life import *
from PIL import Image


def create_gif_from_pgm_files(file_list, output_file):
    history = []
    for file in file_list:
        image = load_pgm_snapshot(file)
        pil_image = Image.fromarray(image.astype(np.uint8))
        history.append(pil_image)

    history[0].save(output_file, save_all=True, append_images=history[1:], duration=400, loop=0)


if __name__ == '__main__':
    path = '../../snapshot'
    file_list = list(map(lambda file: os.path.join(path, file), sorted(os.listdir(path))))
    output_file = '../../simulation/simulation.gif'
    create_gif_from_pgm_files(file_list, output_file)
