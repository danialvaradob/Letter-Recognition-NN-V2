import os
import numpy as np
import random
import PIL
from PIL import Image
from cv2 import cv2

path_original_imgs = "RawData"
path_treated_imgs = "DataImg"
path_data = "Data"

directories = os.listdir(path_original_imgs)

for directory in directories:
    images = os.listdir(path_original_imgs + "/" + directory)
    print("Len: ", len(images))
    f= open(path_data + "/" + directory + ".txt","w")
    for image in images:
        img = Image.open(path_original_imgs + "/" + directory + "/" + image)
        img = img.resize((28, 28), PIL.Image.ANTIALIAS)
        img = img.convert("L")
        img.save(path_treated_imgs + "/" + directory + "/" + image)
        img = cv2.imread(path_treated_imgs + "/" + directory + "/" + image, 0)
        img_reverted= cv2.bitwise_not(img)
        new_img = img_reverted / 255.0
        new_img = np.round(new_img, 1)
        for num in np.nditer(new_img): 
            f.write(str(num) + " ")
        f.write("\r\n")
    f.close()
    



