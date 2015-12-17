#!/usr/bin/env python

import os

dirname = "upi"
os.system("mkdir %s" % dirname)
photoDir = os.getcwd() + "/" + dirname
print(photoDir)

f = open("FN-UPI-list.txt", "r")

while True:
	line = f.readline()
	if line:
		photoUrl = "http://faces.all.alcatel-lucent.com/l/" + line.strip('\n\r')
		photoFile = photoDir + "/" + line.strip('\n\r') + ".JPG"
		flag = os.system("wget -O %s %s" % (photoFile, photoUrl))
		if(flag):
			break
	else:
		break
f.close()
