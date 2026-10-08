#!/usr/bin/env python3

"""

The distortion coefficients is applied to the points after they have been through the perspective divide

Code is a self-contained version of
https://pypi.org/project/vcam/
https://github.com/kaustubh-sadekar/VirtualCam

"""

import cv2
import numpy as np
import math


class vcam:
	def __init__(self, H=400, W=400):
		"""
		H : Desired height of the frame of output video
		W : Desired width of the frame of output
		"""
		self.H = H
		self.W = W
		self.ox = W//2
		self.oy = H//2

		# Euler angles in X, Y, Z
		self.alpha = math.radians(0)
		self.beta = math.radians(0)
		self.gamma = math.radians(0)

		# Translation in X, Y, Z
		self.Tx = 0
		self.Ty = 0
		self.Tz = 0

		self.K = 0  # Intrinsic matrix
		self.RT = 0  # Extrinsic matrix (Rotation + Translation)
		# Rotation matrix used to construct the extrinsic matrix (Uses alpha, beta, gamma)
		self.R = 0
		self.M1 = 0 # Translation matrix used to construct the extrinsic matrix (Uses Tx, Ty, Tz)

		self.sh = 0 # Shear factor for the intrinsic matrix

		self.P = 0
		self.KpCoeff = np.array([0,0,0,0,0,0,0,0]) # k1,k2,p1,p2,k3,k4,k5,k6
		self.focus = 100 # Focal length of camera in mm
		self.sx = 1 # Effective size of a pixel in mm
		self.sy = 1 # Effective size of a pixel in mm
		self.set_tvec(0,0,-self.focus)
		self.update_M()

	def update_M(self):
		# Euler angles
		Rx = np.array([[1, 0, 0], [0, math.cos(self.alpha), -math.sin(self.alpha)], [0, math.sin(self.alpha), math.cos(self.alpha)]])
		Ry = np.array([[math.cos(self.beta), 0, -math.sin(self.beta)], [0, 1, 0], [math.sin(self.beta), 0, math.cos(self.beta)]])
		Rz = np.array([[math.cos(self.gamma), -math.sin(self.gamma), 0], [math.sin(self.gamma), math.cos(self.gamma), 0], [0, 0, 1]])
		# Create the extrinsic rotation matrix by combining the Euler rotation matrices together
		self.R = np.matmul(Rx, np.matmul(Ry, Rz))

		# Intrinsic matrix
		self.K = np.array([[-self.focus/self.sx, self.sh, self.ox],[0,self.focus/self.sy,self.oy],[0,0,1]])

		# Extrinsic matrix
		self.M1 = np.array([[1,0,0,-self.Tx],[0,1,0,-self.Ty],[0,0,1,-self.Tz]])
		self.RT = np.matmul(self.R,self.M1)

	def project(self,src):
		self.update_M()
		pts2d = np.matmul(self.RT,src)

		try:
			# perspective division
			x_1 = pts2d[0,:]*1.0/(pts2d[2,:]+0.0000000001)
			y_1 = pts2d[1,:]*1.0/(pts2d[2,:]+0.0000000001)

			# apply distortion using the intrinsic matrix + distortion coefficients
			r_2 = x_1**2 + y_1**2
			r_4 = r_2**2
			r_6 = r_2**3
			K = (1+self.KpCoeff[0]*r_2+self.KpCoeff[1]*r_4+self.KpCoeff[4]*r_6)/((1+self.KpCoeff[5]*r_2+self.KpCoeff[6]*r_4+self.KpCoeff[7]*r_6))
			x_2 = x_1*K + 2*self.KpCoeff[2]*x_1*y_1 + self.KpCoeff[3]*(r_2+2*x_1**2)
			y_2 = y_1*K + self.KpCoeff[2]*(r_2 + 2*y_1**2) + 2*self.KpCoeff[3]*x_1*y_1

			x = self.K[0,0]*x_2 + self.K[0,2]
			y = self.K[1,1]*y_2 + self.K[1,2]
		except:
			print("Division by zero!")
			x = pts2d[0,:]*0
			y = pts2d[1,:]*0

		return np.concatenate(([x],[y]))

	def set_tvec(self,x,y,z):
		self.Tx = x
		self.Ty = y
		self.Tz = z
		self.update_M()

	def set_rvec(self,alpha,beta,gamma):
		self.alpha = (alpha/180.0)*np.pi
		self.beta = (beta/180.0)*np.pi
		self.gamma = (gamma/180.0)*np.pi
		self.update_M()

	def renderMesh(self,src):
		"""
		Renders the mesh grid points to get better visual understanding
		"""
		self.update_M()
		pts = self.project(src)
		canvas = np.zeros((self.H,self.W,3),dtype=np.uint8)
		pts = (pts.T).reshape(-1,1,2).astype(np.int32)
		cv2.drawContours(canvas,pts,-1,(0,255,0),3)
		return canvas

	def applyMesh(self,img,meshPts):
		pts1,pts2 = np.split(self.project(meshPts),2)
		x = pts1.reshape(self.H,self.W)
		y = pts2.reshape(self.H,self.W)
		return cv2.remap(img,x.astype(np.float32),y.astype(np.float32),interpolation=cv2.INTER_LINEAR)

	def getMaps(self,pts2d):
		pts1,pts2 = np.split(pts2d,2)
		x = pts1.reshape(self.H,self.W)
		y = pts2.reshape(self.H,self.W)

		return x.astype(np.float32),y.astype(np.float32)


class meshGen:

	def __init__(self,H,W):

		self.H = H
		self.W = W

		x = np.linspace(-self.W/2, self.W/2, self.W)
		y = np.linspace(-self.H/2, self.H/2, self.H)

		xv,yv = np.meshgrid(x,y)

		self.X = xv.reshape(-1,1)
		self.Y = yv.reshape(-1,1)
		self.Z = self.X*0+1 # The mesh will be located on Z = 1 plane

	def getPlane(self):

		return np.concatenate(([self.X],[self.Y],[self.Z],[self.X*0+1]))[:,:,0]


def nothing(x):
    pass

WINDOW_NAME = "output"
cv2.namedWindow(WINDOW_NAME,cv2.WINDOW_NORMAL)
cv2.resizeWindow(WINDOW_NAME,700,700)

# Creating the tracker bar for all the features
cv2.createTrackbar("X",WINDOW_NAME,500,1000,nothing)
cv2.createTrackbar("Y",WINDOW_NAME,500,1000,nothing)
cv2.createTrackbar("Z",WINDOW_NAME,0,1000,nothing)
cv2.createTrackbar("alpha",WINDOW_NAME,180,360,nothing)
cv2.createTrackbar("beta",WINDOW_NAME,180,360,nothing)
cv2.createTrackbar("gama",WINDOW_NAME,180,360,nothing)
cv2.createTrackbar("K1",WINDOW_NAME,0,100000,nothing)
cv2.createTrackbar("K2",WINDOW_NAME,0,100000,nothing)
cv2.createTrackbar("P1",WINDOW_NAME,0,100000,nothing)
cv2.createTrackbar("P2",WINDOW_NAME,0,100000,nothing)
cv2.createTrackbar("focus",WINDOW_NAME,600,1000,nothing)
cv2.createTrackbar("Sx",WINDOW_NAME,100,1000,nothing)
cv2.createTrackbar("Sy",WINDOW_NAME,100,1000,nothing)

# cap = cv2.VideoCapture(0)
# ret,img = cap.read()
img = cv2.imread("chess.png")
H,W = img.shape[:2]

c1 = vcam(H=H,W=W)
plane = meshGen(H,W)

plane.Z = plane.X*0 + 1

pts3d = plane.getPlane()


while True:
	# ret, img = cap.read()
	img = cv2.imread("chess.png")
	X = -cv2.getTrackbarPos("X",WINDOW_NAME) + 500
	Y = -cv2.getTrackbarPos("Y",WINDOW_NAME) + 500
	Z = -cv2.getTrackbarPos("Z",WINDOW_NAME)
	alpha = cv2.getTrackbarPos("alpha",WINDOW_NAME) - 180
	beta = cv2.getTrackbarPos("beta",WINDOW_NAME) - 180
	gamma = -cv2.getTrackbarPos("gama",WINDOW_NAME) - 180
	c1.focus = cv2.getTrackbarPos("focus",WINDOW_NAME) - 500
	c1.sx = (cv2.getTrackbarPos("Sx",WINDOW_NAME)+1)/100
	c1.sy = (cv2.getTrackbarPos("Sy",WINDOW_NAME)+1)/100
	k1 = cv2.getTrackbarPos("K1",WINDOW_NAME)/100000
	k2 = cv2.getTrackbarPos("K2",WINDOW_NAME)/100000
	p1 = cv2.getTrackbarPos("P1",WINDOW_NAME)/100000
	p2 = cv2.getTrackbarPos("P2",WINDOW_NAME)/100000
	c1.KpCoeff[0] = k1
	c1.KpCoeff[1] = k2
	c1.KpCoeff[2] = p1
	c1.KpCoeff[3] = p2

	c1.set_tvec(X,Y,Z)
	c1.set_rvec(alpha,beta,gamma)
	pts2d = c1.project(pts3d)
	map_x,map_y = c1.getMaps(pts2d)
	output = cv2.remap(img,map_x,map_y,interpolation=cv2.INTER_LINEAR)

	cv2.imshow("output",output)
	code = cv2.waitKey(1)
	if code == 27 or code == 'q' or code == 'Q':
		break

