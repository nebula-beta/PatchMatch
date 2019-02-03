#!/usr/bin/python2.7
#coding:utf-8


import numpy as np
import matplotlib.pyplot as plt


# spatial propagation & plane refinemnt & view propagation & post-processing(fill invalid pixel and weighted median filter)

x = np.array([1, 2, 3, 4, 5, 6, 7, 8, 9, 10])

# 11px and 31px

y10_left = np.array([12.673587,10.535477, 10.632952, 10.839293, 10.472181, 10.808279, 10.734857, 10.936769, 10.820938, 10.701943])
y10_right = np.array([12.897652,11.349453, 11.453257, 11.308944, 11.496297, 11.564023, 11.510855, 11.528578, 11.576682, 11.389328])

y10_left_31px = [12.672321,12.045066, 11.979872, 11.920375, 11.934299, 11.921640, 11.859612, 12.037471, 11.866574, 11.949491]
y10_right_31px = [13.374264,12.295715, 12.180518, 12.140642, 12.259637, 12.114691, 12.095069, 12.115324, 12.033673, 12.098867]



def draw(fig_name, x, ys, names):
    plt.figure(num = fig_name)
    plt.plot(x, ys[0], marker='o', color = 'red', mec='r', mfc='w', label= names[0])
    plt.plot(x, ys[1], marker='o', color = "lime", mec='r', mfc='w', label= names[1])
    plt.legend()  # 让图例生效

    plt.xlim(0, 11)  # 限定横轴的范围
    plt.xlabel("iteration") #X轴标签
    plt.ylabel("bad error 1.0") #Y轴标签


fig1 = plt.figure(num = "fig1")
fig2 = plt.figure(num = "fig2")

draw("fig1", x, [y10_left, y10_left_31px], ["win size 11px", "win size 31 px"])

draw("fig2", x, [y10_right, y10_right_31px], ["win size 11px", "win size 31 px"])
plt.show()
