#import cv2
#import numpy as np
#import matplotlib.pyplot as plt

#image = cv2.imread("DAA_road.jpg")
#print(image.shape)
#plt.imshow(image)
#plt.show()

import cv2
import numpy as np

def make_coordinates(image,line_parameters):
    slope,intercept = line_parameters
    y1 = image.shape[0]
    y2 = int(y1*(3/5))
    x1 = int((y1-intercept)/slope)
    x2 = int((y2-intercept)/slope)
    return np.array([x1,y1,x2,y2])

def average_slope_intercept(image,lines):
    left_fit = []
    right_fit = []
    for line in lines :
        x1,y1,x2,y2 = line.reshape(4)
        paramerters = np.polyfit((x1,x2),(y1,y2),1)
        slope = paramerters[0]
        intercept = paramerters[1]
        if slope < 0:
            left_fit.append((slope,intercept))
        else:
            right_fit.append((slope,intercept))
    left_fit_average = np.average(left_fit,axis=0)
    right_fit_average = np.average(right_fit,axis=0)
    left_line = make_coordinates(image,left_fit_average)
    right_line = make_coordinates(image,right_fit_average)
    return np.array([left_line,right_line])

def canny(image):
    gray = cv2.cvtColor(image,cv2.COLOR_RGB2GRAY)
    blur = cv2.GaussianBlur(gray,(5,5),0)
    canny = cv2.Canny(blur,50,150)
    return canny
def display_line(image,lines):
    line_image = np.zeros_like(image)
    if lines is not None:
        for line in lines:
             x1,y1,x2,y2 = line.reshape(4)
             cv2.line(line_image,(x1,y1),(x2,y2),(255,0,0),10)
    return line_image

def region_of_interest(image):
    height = image.shape[0]
    polygons = np.array([
    [(200,height),(1100,height),(550,250)]
    ])
    mask = np.zeros_like(image)
    cv2.fillPoly(mask,polygons,255)
    masked_image = cv2.bitwise_and(image,mask)
    return masked_image

image = cv2.imread("2.png")
lane_image = np.copy(image)
canny_image = canny(lane_image)
height = image.shape[0]
polygons = np.array([
[(200,height),(1100,height),(550,250)]
])
mask = np.zeros_like(image)
cv2.fillPoly(mask,polygons,255)
masked_image = cv2.bitwise_and(image,mask)
gray = cv2.cvtColor(image,cv2.COLOR_RGB2GRAY)
blur = cv2.GaussianBlur(gray,(25,25),0)
cropped_image = region_of_interest(canny_image)
lines = cv2.HoughLinesP(cropped_image,2,np.pi/180,100,np.array([]),minLineLength=40,maxLineGap=5)
line_image = display_line(lane_image,lines)
combo_image = cv2.addWeighted(lane_image,0.1,line_image,0.5,1)
cv2.imshow("result",line_image)
cv2.waitKey(0)
