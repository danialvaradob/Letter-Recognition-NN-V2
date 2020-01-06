import PIL
from PIL import Image
from cv2 import cv2
import numpy as np
import random
import os

path_original_imgs = "RawData"
path_treated_imgs = "DataImg"
path_data = "Data"

directoriess = os.listdir(path_original_imgs)

for directories in directoriess:
    images = os.listdir(path_original_imgs + "/" + directories)
    f= open(path_data + "/" + directories + ".txt","w")
    for image in images:
        img = Image.open(path_original_imgs + "/" + directories + "/" + image)
        img = img.resize((28, 28), PIL.Image.ANTIALIAS)
        img = img.convert("L")
        img.save(path_treated_imgs + "/" + directories + "/" + image)
        #cambia a entre 0 y 1
        img = cv2.imread(path_treated_imgs + "/" + directories + "/" + image, 0)
        img_reverted= cv2.bitwise_not(img)
        new_img = img_reverted / 255.0
        
        new_img = np.round(new_img, 1)
        for num in np.nditer(new_img): 
            f.write(str(num) + " ")
        f.write("\r\n")
    f.close()



