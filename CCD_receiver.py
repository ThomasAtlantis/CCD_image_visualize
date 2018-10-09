import numpy as np
import matplotlib.pyplot as plt
import serial

fig, ax = plt.subplots()
ser = serial.Serial("COM5", 9600, timeout=0.5)
img = [[0 for i in range(128)] for j in range(128)]
plt.ion()
while True:
    tmp = ser.readline().strip().split()
    if len(tmp) != 128:
        continue
    data = [1 - int(t.decode()) for t in tmp]
    # print(data)
    img.pop(0)
    img.append(data)
    ax.cla()
    ax.axis('off')
    ax.imshow(np.array(img), cmap=plt.cm.gray_r)
    plt.pause(0.001)
ser.close()

