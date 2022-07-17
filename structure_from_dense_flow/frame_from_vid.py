import cv2
import os
vid = "video/videoplayback_side.mp4"
count=1
from_sec = 100
to_sec = 102

vidcap = cv2.VideoCapture(vid)
def getFrame(sec):
    if sec < from_sec:
        return True
    if sec > to_sec:
        return False
    vidcap.set(cv2.CAP_PROP_POS_MSEC,sec*1000)
    hasFrames,image = vidcap.read()
    if hasFrames:
        cv2.imwrite("videoimage1/"+str(count)+".jpg", image) # Save frame as JPG file
    return hasFrames
sec = 0
frameRate = 0.1 # Change this number to 1 for each 1 second

success = getFrame(sec)
while success:
    count = count + 1
    sec = sec + frameRate
    sec = round(sec, 2)
    success = getFrame(sec)