import numpy as np
import cv2 as cv

img_list0 = ["0001", "0002", "0003", "0004", "0005", "0006", "0007", "0008", "0009", "0010"]
img_list = ["1001.jpg", "1002.jpg", "1003.jpg", "1004.jpg", "1005.jpg", "1006.jpg", "1007.jpg", "1008.jpg", "1009.jpg", "1010.jpg", "1011.jpg", "1012.jpg", "1013.jpg", "1014.jpg", "1015.jpg", "1016.jpg", "1017.jpg", "1018.jpg", "1019.jpg", "1020.jpg", "1021.jpg"]

img_path = "./videoimage1/"
scaled_size = (360, 640)    # y, x
tile = (1, 1)
# principal point
px = scaled_size[1] / 2
py = scaled_size[0] / 2

# Camera setting
f = 800
t = (1.0, 0, 0)

previous = np.zeros(shape=scaled_size)

class FlowVector:
    def __init__(self, x, y, u, v):
        self.x, self.y, self.u, self.v = (x, y, u, v)
    def sqrMag(self):
        return self.u * self.u + self.v * self.v
    def __repr__(self):
        return "x: " + str(self.x) + "  y: " + str(self.y) + "  u: " + str(self.u) + "  v: " + str(self.v)

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
    z_map = np.zeros(shape=flow_size)

    for vector in vector_list:
        # Ranged flow from (-1, 1) to (-128, 128)
        rf = FlowVector(vector.x, vector.y, vector.u * 128, vector.v * 128)
        if rf.u != 0:
            z_map[vector.y][vector.x] = abs((t[0] * f - (rf.x - px) * t[2]) / rf.u - t[2])
        elif rf.v != 0:
            z_map[vector.y][vector.x] = abs((t[1] * f - (rf.y - py) * t[2]) / rf.v - t[2])
        else:
            z_map[vector.y][vector.x] = 255
        
        if z_map[vector.y][vector.x] > 255: z_map[vector.y][vector.x] = 255

        x_map[vector.y][vector.x] = rf.u * z_map[vector.y][vector.x] / f
        y_map[vector.y][vector.x] = rf.v * z_map[vector.y][vector.x] / f
    
    return x_map, y_map, z_map

for i in img_list:
    # Open files and convert to Dense flow
    frame = cv.imread(img_path + i)
    frame = cv.resize(frame, (scaled_size[1], scaled_size[0]))
    hsv = np.zeros_like(frame)
    frame = cv.cvtColor(frame, cv.COLOR_BGR2GRAY)
    flow = cv.calcOpticalFlowFarneback(previous, frame, None, 0.5, 3, 15, 3, 5, 1.2, 0)
    best_flow = best_vector_from_flow(flow)
    xmap, ymap, zmap = structure_from_vector(best_flow, scaled_size)

    # mag, ang = cv.cartToPolar(flow[..., 0], flow[..., 1])
    # hsv[..., 1] = 255
    # hsv[..., 0] = ang*180/np.pi/2
    # hsv[..., 2] = cv.normalize(mag, None, 0, 255, cv.NORM_MINMAX)
    # bgr = cv.cvtColor(hsv, cv.COLOR_HSV2BGR)
    # cv.imshow("win", bgr)

    cv.imshow("win", zmap)

    cv.waitKey(10)
    previous = frame

cv.destroyAllWindows()