import PIL
from PIL import Image
from cv2 import cv2
import numpy as np
import random
import os


path_inicial = "Test/RawData"
path_final = "Test/DataImg"


images = os.listdir(path_inicial)
f= open("Data/imagen_prueba.txt","w")
for image in images:
    img = Image.open(path_inicial + "/" + image)
    img = img.resize((28, 28), PIL.Image.ANTIALIAS)
    img = img.convert("L")
    img.save(path_final + "/" + image)
    img = cv2.imread(path_final + "/" + image, 0)
    img_reverted= cv2.bitwise_not(img)
    new_img = img_reverted / 255.0
    
    new_img = np.round(new_img, 1)
    for num in np.nditer(new_img): 
        f.write(str(num) + " ")
    f.write("\r\n")
    break
f.close()



