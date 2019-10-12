import numpy as np
np.random.seed(0)
from PIL import Image
from io import BytesIO

import os
os.environ['TF_CPP_MIN_LOG_LEVEL'] = '3' 

import tensorflow as tf
import logging
tf.get_logger().setLevel(logging.ERROR)
import pandas as pd # data processing, CSV file I/O (e.g. pd.read_csv)
import h5py
import skimage
import matplotlib.pyplot as plt
import sys

from flask import Flask,request
import requests

app = Flask(__name__)
from tensorflow.keras.applications.resnet50 import ResNet50
from tensorflow.keras.applications.resnet50 import preprocess_input
from tensorflow.keras.applications.vgg16 import VGG16
from tensorflow.keras import optimizers

from tensorflow.keras.models import load_model
from tensorflow.keras.models import model_from_json

from tensorflow.keras.backend import set_session

g_session = tf.Session()
g_graph = tf.get_default_graph()
g_docpath = "../../"
g_initb = False

def py_eval(buff):
	# import cStringIO
	print("PGOT CALLED\n")
	npbuff = np.asarray(buff)
	print(npbuff.shape)
	npbuff = npbuff.reshape((64, 64,3))
	print(npbuff.shape)
	print("PTRANSPOSE")	
	# transpoe
	npbuff = np.flip(npbuff, 2)
	npbuff = np.flip(npbuff, 0)
	print(npbuff.shape)
	print("P AS FLOAT")	
	npbuff = npbuff.astype(float)/255.
	npbuff_expanded = np.expand_dims(npbuff, axis = 0)
	print("PHERE")
	temp =  pyeval_instance._eval(npbuff_expanded)
	print("TEMP")
	print(temp)
	return temp



def init_py_bridge(docpath):
	print("INIT called")
	print("docpath: ")
	print(docpath)
	global pyeval_instance
	pyeval_instance = __fd_pyeval(docpath)
	fil = open("blaboc_init.txt", "w")
	fil.write("bitchhhhhhhh")
	fil.close()


class __fd_pyeval:
	def __init__(self, docpath):
		
		set_session(g_session)
		
		model_path = docpath + "/vgg16_json.json" 
		t_mod_json = open(model_path,"r")
		loaded_mod = t_mod_json.read()
		t_mod_json.close()
		
		self.model = model_from_json(loaded_mod)
		self.model._make_predict_function()
		
		weights_path = docpath + "/np_vgg16_weights.npy"
		self.np_weights = np.load(weights_path, allow_pickle = True)
		self.model.set_weights(self.np_weights)
		
		# load cateogories
		categories_path = docpath + "/categories.txt"
		self.categories = []
		cat_f = open(categories_path, "r")
		for line in cat_f:
			self.categories.append(line)


	def _eval(self, img_np):
		global g_session
		global g_graph
		
		with g_graph.as_default():
			set_session(g_session)
			pred_y = self.model.predict(img_np)
			print("PRED Y")
			k = 0
		
			zero_y = np.zeros(pred_y.shape)
			argmax_lst=np.argmax(pred_y,axis=1)
			print("ARGAMX")
			#print(argmax_lst)
			for i in range(len(argmax_lst)):
				 zero_y[i][argmax_lst[i]]=1
			pred_y=zero_y
			corr_index = np.where(pred_y == 1)
			truth = int(corr_index[1])
			return truth #str(self.categories[truth])

def eval_image(global_bool, url):
	if global_bool == 0: # or sys.argv[2] == "local":
		img = Image.open(url)
	else:
		response = requests.get(url)
		img = Image.open(BytesIO(response.content))
		img = img.resize((64, 64))

	np_ar = np.array(img)
	#np_ar = np_ar.astype(float)/255.
	#np_mod = np.expand_dims(np_ar, axis = 0)

	# return tempinstance._eval(np_mod)
	return py_eval(np_ar)



@app.route("/local/<index>")
def eval_local(index):
	# global g_docpath
	return str(eval_image(0, g_docpath + "data_cropped/"+str(index)))

@app.route("/global/", methods = ["GET", "POST"])
def eval_global():
	if request.method == "GET":
		return "Send URL!!!"
	elif request.method == "POST":
		return str(eval_image(1, request.form["url"]))

# eval_image(sys.argv[1])

tempinstance = None

if __name__ == "__main__":
 	#tempinstance = __fd_pyeval( "../../")
	init_py_bridge("../../")
	# global g_docpath
	# g_docpath = "../../"
	app.run()




