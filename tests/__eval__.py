import numpy as np
np.random.seed(0)
from PIL import Image

def pypy(val):
	print("got_called with :%f" %(val))


def eval(buff):
	import cStringIO
	npbuff = np.asarray(buff)
	npbuff = npbuff.reshape((64, 64,3))
	
# transpoe
	npbuff = np.flip(npbuff, 2)
	npbuff = np.flip(npbuff, 0)
	print(npbuff.shape)
	
	img = Image.fromarray(npbuff.astype('uint8'))
	# img.resize((64, 64))
	if img.mode != 'RGB':
		img = img.convert('RGB')
	img.save("savedfrompython.png")
	print("done")

def another_test():
	img = Image.open("test_files/lena.bmp")
	temp = np.array(img)
	print(temp.shape)
	print()

	for i in range(0,5):
		print(temp[i])




#if __name__ == '__main__':
	#another_test()

