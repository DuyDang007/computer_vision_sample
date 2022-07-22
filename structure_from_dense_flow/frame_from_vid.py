import os, sys
import cv2

vid = os.path.abspath(sys.argv[1])
output_dir = os.path.abspath(sys.argv[2])
from_sec = int(sys.argv[3])
to_sec = int(sys.argv[4])
frameRate = 0.1 # Change this number to 1 for each 1 second

count=1
vidcap = cv2.VideoCapture(vid)
def getFrame(sec):
    if sec < from_sec:
        return True
    if sec > to_sec:
        return False
    vidcap.set(cv2.CAP_PROP_POS_MSEC,sec*1000)
    hasFrames,image = vidcap.read()
    if hasFrames:
        cv2.imwrite(os.path.join(output_dir, str(count) + ".jpg"), image) # Save frame as JPG file
    return hasFrames
sec = 0

success = getFrame(sec)
while success:
    count = count + 1
    sec = sec + frameRate
    sec = round(sec, 2)
    success = getFrame(sec)