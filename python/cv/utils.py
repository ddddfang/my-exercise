
import numpy as np

def salt(img, n):
    img_copy = img.copy()
    for k in range(n):
        i = int(np.random.random() * img_copy.shape[0]);
        j = int(np.random.random() * img_copy.shape[1]);
        if img_copy.ndim == 2: 
            img_copy[i, j] = 255
        elif img.ndim == 3: 
            img_copy[i, j, 0]= 255
            img_copy[i, j, 1]= 255
            img_copy[i, j, 2]= 255
    return img_copy
