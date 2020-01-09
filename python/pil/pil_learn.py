from PIL import Image
from matplotlib import pyplot as plt

pil_im = Image.open('empire.jpg')
pil_im_gray = pil_im.convert('L')

pil_im.thumbnail((128,128))

box = (100,100,400,400)
region = pil_im.crop(box)

region = region.transpose(Image.ROTATE_180)
pil_im.paste(region, box)

out = pil_im.resize((128,128))
out = pil_im.rotate(45)


