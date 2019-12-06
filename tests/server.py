#!/usr/bin/python3

from ctypes import *
# from pybufftest import *

# lib_file = "/home/bhavaygrover/Desktop/fd/openfd/libopenfd.so"
lib_file = "/Users/bhavaygrover/Desktop/mainopenfd/fresh/openfd/tests/libopenfd.so" 

openfd = CDLL(lib_file)

class INT2(Structure):
	_fields_ = ("x", c_int), ("y", c_int)

 
class FLOAT2(Structure):
	_fields_ = ("x", c_float), ("y", c_float)


class FD_CONFIG_T(Structure):
	pass

class FOOD_POS_T(Structure):
	pass


class FD_RESULT_T(Structure):
	pass



FD_CONFIG_T._fields_ = ("indims", INT2), ("format", c_int), ("docpath", c_char_p  )


FOOD_POS_T._fields_ = ("food_name", c_char * 100), ("pos_topright", INT2), ("pos_bottomleft", INT2), ("projected_food_pos", INT2), ("__confidence", c_double)


FD_RESULT_T._fields_ = ("frame_id", c_int), ("timestamp", c_ulong), ("num_fooditems", c_int), ("fooditems", FOOD_POS_T * 80)

from flask import Flask, request, jsonify
from flask_cors import CORS, cross_origin 

import requests

from io import BytesIO

import numpy as np
import json 
from PIL import Image

_g_res = FD_RESULT_T()


def is_valid_url(path):
    r = requests.head(path)
    return r.status_code == requests.codes.ok


def get_py_buff(im):
	# im = Image.open(r"pizza.bmp")
	im.save("httpreceived.bmp")
	
	newsize = (416, 416)
	imsz = im.resize(newsize)
	imsz.save("resized.bmp")

	imgb = imsz.tobytes("xbm", "rgb")
# img = Image.frombuffer("L", (416,416), imgb, "raw", "L", 0, 1)

	pix = np.array(imsz)
	pix = pix.reshape((416, 416, 3))
	pix = np.flip(pix, 2)
	pix = np.flip(pix, 0)
	im2 = Image.fromarray(pix.astype('uint8'), 'RGB')
	im2.save("flipped.bmp")


	bufflis = list(im2.getdata())
	buffil = open("pybuffer.txt", "w")
	
	retlist = []

	for i in bufflis:
		for j in range(0, 3):
			retlist.append(i[j])
			buffil.write( "%d," % i[j])
		
	buffil.close()
	return retlist


def fd_make_json(fd_res):
	retj = "{\"frame_id\":" + str(fd_res.frame_id) + ",\"timestamp\":" + str(fd_res.timestamp) + ",\"num_fooditems\":" + str(fd_res.num_fooditems) + ",\"fooditems\":["

	for i in range(0,fd_res.num_fooditems):
		j = fd_res.fooditems[i]
		retj = retj + "{\"food_name\":\"" + str(j.food_name, "utf-8")+ "\",\"pos_topright\":{\"x\":" + str(j.pos_topright.x) + ",\"y\":" + str(j.pos_topright.y) + "}" + ",\"pos_bottomleft\":{\"x\":" + str(j.pos_bottomleft.x) + ",\"y\":" + str(j.pos_bottomleft.y) + "},\"confidence\": " + str(j.__confidence) + "}"
		if i != fd_res.num_fooditems-1:
			retj = retj + ","
	
	retj =  retj + "]}"
	return retj

def run_openfd(url):
	try:
		url_res = requests.get(url)
		img = Image.open(BytesIO(url_res.content))
		pybuff = get_py_buff(img)  # py list 
		sz = 416 * 416 * 3

		carr = (c_ubyte * sz)(*pybuff)
		buffptr = cast(carr, c_void_p)

		frameid = 0
		# res = FD_RESULT_T()
		global _g_res
		openfd.fd_get_result_sync(c_int(frameid), buffptr, byref(_g_res))
		print("received num food items: ")
		print(_g_res.num_fooditems)
		jsonout = fd_make_json(_g_res)
		return jsonout
	except:
		return None

app = Flask(__name__)
app.config['CORS_HEADERS'] = "Content-Type"
cors = CORS(app, resources={r"/evaluate/": {"origins": "*"}}) # "http://127.0.0.1:5000"}})

@app.route("/")
def home():
	return "openfd server running"

@app.route("/evaluate/", methods=['POST'] )
def evaluate():
	print("form received: ")
	print(request.form.to_dict())
	ctx_url = request.form.get("url")
	print("url: ")
	print(ctx_url)
	#if is_valid_url(ctx_url):
	jsonres = run_openfd(request.form.get("url"))
	if jsonres == None:
		err_res = "{\"error\": \"invalid_url\"}"		
		return err_res , 400, {'Content-Type':"application/json" }
	return jsonres, 200, {'Content-Type':"application/json", "Access-Control-Allow-Origin": "*" }
	# run_openfd(url)
	#else:
	#	err_res = "{\"error\": \"invalid_url\"}"		
	#return err_res , 400, {'Content-Type':"application/json" }


if __name__ == "__main__":
	openfd.fd_init.restype = c_int

	inputdims = INT2(416 , 416)
	config = FD_CONFIG_T(inputdims, 0, c_char_p("/home/bhavaygrover/Desktop/fd/openfd/docs/".encode('utf-8')))

	openfd.fd_init(config)

	app.run()

#print(vars(openfd))


