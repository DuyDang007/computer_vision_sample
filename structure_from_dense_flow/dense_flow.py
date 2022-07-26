import os
import numpy as np
import cv2 as cv
import math

class TranslateVector:
    def __init__(self, x, y, z):
        self.x, self.y, self.z = (x, y, z)

class FlowVector:
    def __init__(self, x, y, u, v):
        self.x, self.y, self.u, self.v = (x, y, u, v)
    def sqrMag(self):
        return self.u * self.u + self.v * self.v
    def __repr__(self):
        return "x: " + str(self.x) + "  y: " + str(self.y) + "  u: " + str(self.u) + "  v: " + str(self.v)

##############################################
img_path = "../videoimage2/"
img_list = [f for f in os.listdir(img_path) if os.path.isfile(os.path.join(img_path, f))]

scaled_size = (240, 320)    # y, x
tile = (2, 2)
# principal point
px = scaled_size[1] / 2
py = scaled_size[0] / 2

# Camera setting
f = 800
trans = TranslateVector(0.0, 0.0, -1.0)

##############################################
def best_vector_from_flow(flow_img) -> list:
    size_x = len(flow_img[0])
    size_y = len(flow_img)
    best_flow_list = []

    for y in range(0, size_y, tile[1]):
        for x in range(0, size_x, tile[0]):
            for tile_y in range(tile[1]):
                best_vector = FlowVector(x, y, flow_img[y][x][0], flow_img[y][x][1])
                for tile_x in range(tile[0]):
                    if x + tile_x >= size_x: break
                    v = FlowVector(x + tile_x, y + tile_y, flow_img[y + tile_y][x + tile_x][0], flow_img[y + tile_y][x + tile_x][1])
                    if v.sqrMag() > best_vector.sqrMag():
                        best_vector = v
                if y + tile_y >= size_y: break

            best_flow_list.append(best_vector)
    return best_flow_list

def structure_from_vector(vector_list, flow_size:tuple) -> tuple:
    x_map = np.zeros(shape=flow_size)
    y_map = np.zeros(shape=flow_size)
    z_map = np.full(shape=flow_size, fill_value=32768.0)

    for vector in vector_list:
        # Ranged flow from (-1, 1) to (-128, 128)
        rf = FlowVector(vector.x, vector.y, vector.u * 128.0, vector.v * 128.0)
        if rf.u != 0:
            z_map[vector.y][vector.x] = (trans.x * f - trans.z * (px - (rf.x + rf.u))) / rf.u
        elif rf.v != 0:
            z_map[vector.y][vector.x] = (trans.y * f - trans.z * (py - (rf.y + rf.v))) / rf.v
        
        if z_map[vector.y][vector.x] > 32768.0:
            z_map[vector.y][vector.x] = 32768.0
        if z_map[vector.y][vector.x] < -32768.0:
            z_map[vector.y][vector.x] = -32768.0

        x_map[vector.y][vector.x] = (px - rf.x) * abs(z_map[vector.y][vector.x]) / f
        y_map[vector.y][vector.x] = (py - rf.y) * z_map[vector.y][vector.x] / f
    
    return x_map, y_map, z_map

##################################### MAIN ######################################
previous = np.zeros(shape=scaled_size)
previous_flow = np.zeros(shape=scaled_size)
for i in img_list:
    # Open files and convert to Dense flow
    frame = cv.imread(img_path + i)
    frame = cv.resize(frame, (scaled_size[1], scaled_size[0]))
    hsv = np.zeros_like(frame)
    frame_gray = cv.cvtColor(frame, cv.COLOR_BGR2GRAY)
    flow = cv.calcOpticalFlowFarneback(previous, frame_gray, previous_flow, 0.5, 3, 15, 3, 5, 1.2, 0)
    previous = frame_gray
    previous_flow = flow
    # Extract best vectors
    best_flow = best_vector_from_flow(flow)
    xmap, ymap, zmap = structure_from_vector(best_flow, scaled_size)


    # Create RGB image of Z
    bgr_zmap = np.zeros(shape=(scaled_size[0], scaled_size[1], 3))
    for y in range(scaled_size[0]):
        for x in range(scaled_size[1]):
            arctaned = round(math.atan(abs(zmap[y][x])*0.5) * 162.0)
            if zmap[y][x] < 0:
                bgr_zmap[y][x] = (arctaned, arctaned, arctaned)
            else:
                bgr_zmap[y][x] = (arctaned, arctaned, arctaned)
    bgr_zmap = bgr_zmap.astype(np.uint8)

    # Create BGR image of X
    bgr_xmap = np.zeros(shape=(scaled_size[0], scaled_size[1], 3))
    for y in range(scaled_size[0]):
        for x in range(scaled_size[1]):
            arctaned = round(math.atan(abs(xmap[y][x]*20.0)) * 162.0)
            if xmap[y][x] < 0:
                bgr_xmap[y][x] = (0, 0, arctaned)
            else:
                bgr_xmap[y][x] = (0, arctaned, 0)
            # print(xmap[y][x])
            

    mag, ang = cv.cartToPolar(flow[..., 0], flow[..., 1])
    hsv[..., 1] = 255
    hsv[..., 0] = ang*180/np.pi/2
    hsv[..., 2] = cv.normalize(mag, None, 0, 255, cv.NORM_MINMAX)
    bgr_flow = cv.cvtColor(hsv, cv.COLOR_HSV2BGR)

    img_final_1 = np.hstack((frame, bgr_flow)).astype(np.uint8)
    img_final_2 = np.hstack((bgr_xmap, bgr_zmap)).astype(np.uint8)
    img_final = np.vstack((img_final_1, img_final_2))
    cv.imshow("win", img_final)

    # cv.imshow("win", bgr_xmap.astype(np.uint8))

    cv.waitKey(1)

cv.destroyAllWindows()